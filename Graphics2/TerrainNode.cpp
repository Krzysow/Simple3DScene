#include "TerrainNode.h"

struct CBUFFER
{
	XMMATRIX    CompleteTransformation;
	XMMATRIX	WorldTransformation;
	XMFLOAT4	CameraPosition;
	XMVECTOR    LightVector;
	XMFLOAT4    LightColor;
	XMFLOAT4    AmbientColor;
	XMFLOAT4    DiffuseCoefficient;
	XMFLOAT4	SpecularCoefficient;
	float		Shininess;
	float		Opacity;
	float       Padding[2];
};

TerrainNode::TerrainNode(wstring name, wstring heightMapFilename) : SceneNode(name)
{
	_heightMapFilename = heightMapFilename;
}

TerrainNode::~TerrainNode()
{
}

bool TerrainNode::Initialise()
{
	LoadHeightMap(_heightMapFilename);
	BuildGeometryBuffers();
	LoadTerrainTextures();
	GenerateBlendMap();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	//BuildRendererStates();

	return true;
}

void TerrainNode::Render()
{
	// Calculate the world x view x projection transformation
	XMMATRIX completeTransformation = XMLoadFloat4x4(&_combinedWorldTransformation) * DirectXFramework::GetDXFramework()->GetCamera()->GetViewMatrix() * DirectXFramework::GetDXFramework()->GetProjectionTransformation();

	CBUFFER cBuffer;
	cBuffer.CompleteTransformation = completeTransformation;
	cBuffer.WorldTransformation = XMLoadFloat4x4(&_combinedWorldTransformation);
	XMStoreFloat4(&cBuffer.CameraPosition, DirectXFramework::GetDXFramework()->GetCamera()->GetCameraPosition());
	cBuffer.LightVector = XMVector4Normalize(XMVectorSet(-1.0f, -1.0f, 1.0f, 0.0f));
	cBuffer.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cBuffer.AmbientColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	cBuffer.DiffuseCoefficient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cBuffer.SpecularCoefficient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	cBuffer.Shininess = 1.0f;
	cBuffer.Opacity = 1.0f;

	_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);
	_deviceContext->IASetInputLayout(_layout.Get());

	// Update the constant buffer 
	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);

	// Set the texture to be used by the pixel shader
	_deviceContext->PSSetShaderResources(0, 1, _blendMapResourceView.GetAddressOf());
	_deviceContext->PSSetShaderResources(1, 1, _texturesResourceView.GetAddressOf());

	// Now render the grid
	UINT stride = sizeof(TerrainVertex);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//_deviceContext->RSSetState(_wireframeRasteriserState.Get());
	_deviceContext->DrawIndexed(static_cast<UINT>(_indices.size()), 0, 0);
}

void TerrainNode::BuildGeometryBuffers()
{
	UINT index = 0;
	for (unsigned int i = 0; i < _numberOfRows; i++)
	{
		for (unsigned int j = 0; j < _numberOfColumns; j++)
		{
			// Create vertex buffer
			float x = _terrainStartX + _spacing * j;
			float z = _terrainStartZ - _spacing * i;
			_vertices.push_back({ XMFLOAT3(x, _heightValues[i * _numberOfXPoints + j] * _worldMaxHeight, z),
								  XMFLOAT3(0.0f, 0.0f, 0.0f),
								  XMFLOAT2(0.0f, 0.0f),
								  XMFLOAT2(static_cast<float>(j) / static_cast<float>(_numberOfXPoints), static_cast<float>(i) / static_cast<float>(_numberOfZPoints)) });
			_vertices.push_back({ XMFLOAT3(x + _spacing, _heightValues[i * _numberOfXPoints + j + 1] * _worldMaxHeight, z),
								  XMFLOAT3(0.0f, 0.0f, 0.0f),
								  XMFLOAT2(1.0f, 0.0f),
								  XMFLOAT2(static_cast<float>(j + 1) / static_cast<float>(_numberOfXPoints), static_cast<float>(i) / static_cast<float>(_numberOfZPoints)) });
			_vertices.push_back({ XMFLOAT3(x, _heightValues[(i + 1) * _numberOfXPoints + j] * _worldMaxHeight, z - _spacing),
								  XMFLOAT3(0.0f, 0.0f, 0.0f),
								  XMFLOAT2(0.0f, 1.0f),
								  XMFLOAT2(static_cast<float>(j) / static_cast<float>(_numberOfXPoints), static_cast<float>(i + 1) / static_cast<float>(_numberOfZPoints)) });
			_vertices.push_back({ XMFLOAT3(x + _spacing, _heightValues[(i + 1) * _numberOfXPoints + j + 1] * _worldMaxHeight, z - _spacing),
								  XMFLOAT3(0.0f, 0.0f, 0.0f),
								  XMFLOAT2(1.0f, 1.0f),
								  XMFLOAT2(static_cast<float>(j + 1) / static_cast<float>(_numberOfXPoints), static_cast<float>(i + 1) / static_cast<float>(_numberOfZPoints)) });

			// Create the index buffer
			_indices.push_back(index);
			_indices.push_back(index + 1);
			_indices.push_back(index + 2);
			_indices.push_back(index + 2);
			_indices.push_back(index + 1);
			_indices.push_back(index + 3);

			index += 4;
		}
	}
	
	index = 0;
	for (unsigned int i = 0; i < _numberOfRows; i++)
	{
		for (unsigned int j = 0; j < _numberOfColumns; j++)
		{
			// Calculating normals
			XMVECTOR v1 = XMLoadFloat3(&_vertices[index].Position);
			XMVECTOR v2 = XMLoadFloat3(&_vertices[index + 1].Position);
			XMVECTOR v3 = XMLoadFloat3(&_vertices[index + 2].Position);
			XMVECTOR normal = XMVector3Cross(v2 - v1, v3 - v1);

			XMStoreFloat3(&_vertices[index].Normal, XMLoadFloat3(&_vertices[index].Normal) + normal);
			XMStoreFloat3(&_vertices[index + 1].Normal, XMLoadFloat3(&_vertices[index + 1].Normal) + normal);
			XMStoreFloat3(&_vertices[index + 2].Normal, XMLoadFloat3(&_vertices[index + 2].Normal) + normal);
			XMStoreFloat3(&_vertices[index + 3].Normal, XMLoadFloat3(&_vertices[index + 3].Normal) + normal);

			if (j > 0)
			{
				XMStoreFloat3(&_vertices[index - 3].Normal, XMLoadFloat3(&_vertices[index - 3].Normal) + normal);
				XMStoreFloat3(&_vertices[index - 1].Normal, XMLoadFloat3(&_vertices[index - 1].Normal) + normal);

				if (i > 0)
				{
					XMStoreFloat3(&_vertices[index - 1 - 4 * _numberOfColumns].Normal, XMLoadFloat3(&_vertices[index - 1 - 4 * _numberOfColumns].Normal) + normal);
				}

				if (i < _numberOfRows - 1)
				{
					XMStoreFloat3(&_vertices[index - 3 + 4 * _numberOfColumns].Normal, XMLoadFloat3(&_vertices[index - 3 + 4 * _numberOfColumns].Normal) + normal);
				}
			}

			if (i > 0)
			{
				XMStoreFloat3(&_vertices[index + 2 - 4 * _numberOfColumns].Normal, XMLoadFloat3(&_vertices[index + 2 - 4 * _numberOfColumns].Normal) + normal);
				XMStoreFloat3(&_vertices[index + 3 - 4 * _numberOfColumns].Normal, XMLoadFloat3(&_vertices[index + 3 - 4 * _numberOfColumns].Normal) + normal);
			}

			if (j < _numberOfColumns - 1)
			{
				XMStoreFloat3(&_vertices[index + 4].Normal, XMLoadFloat3(&_vertices[index + 4].Normal) + normal);
				XMStoreFloat3(&_vertices[index + 6].Normal, XMLoadFloat3(&_vertices[index + 6].Normal) + normal);

				if (i > 0)
				{
					XMStoreFloat3(&_vertices[index + 6 - 4 * _numberOfColumns].Normal, XMLoadFloat3(&_vertices[index + 6 - 4 * _numberOfRows].Normal) + normal);
				}

				if (i < _numberOfRows - 1)
				{
					XMStoreFloat3(&_vertices[index + 4 + 4 * _numberOfColumns].Normal, XMLoadFloat3(&_vertices[index + 4 + 4 * _numberOfColumns].Normal) + normal);
				}
			}

			if (i < _numberOfRows - 1)
			{
				XMStoreFloat3(&_vertices[index + 4 * _numberOfColumns].Normal, XMLoadFloat3(&_vertices[index + 4 * _numberOfColumns].Normal) + normal);
				XMStoreFloat3(&_vertices[index + 1 + 4 * _numberOfColumns].Normal, XMLoadFloat3(&_vertices[index + 1 + 4 * _numberOfColumns].Normal) + normal);
			}

			index += 4;
		}
	}

	for (TerrainVertex& vertex : _vertices)
	{
		XMStoreFloat3(&vertex.Normal, XMVector3Normalize(XMLoadFloat3(&vertex.Normal)));
	}
	
	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(TerrainVertex) * static_cast<UINT>(_vertices.size());
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the vertices from
	D3D11_SUBRESOURCE_DATA vertexInitialisationData;
	vertexInitialisationData.pSysMem = &_vertices[0];

	// and create the vertex buffer
	ThrowIfFailed(_device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));

	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor;
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * static_cast<UINT>(_indices.size());
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = &_indices[0];

	// and create the index buffer
	ThrowIfFailed(_device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));
}

void TerrainNode::BuildShaders()
{
	DWORD shaderCompileFlags = 0;
#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(L"TerrainShaders.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VShader", "vs_5_0",
		shaderCompileFlags, 0,
		_vertexShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	// Even if there are no compiler messages, check to make sure there were no other errors.
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreateVertexShader(_vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), NULL, _vertexShader.GetAddressOf()));

	// Compile pixel shader
	hr = D3DCompileFromFile(L"TerrainShaders.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PShader", "ps_5_0",
		shaderCompileFlags, 0,
		_pixelShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreatePixelShader(_pixelShaderByteCode->GetBufferPointer(), _pixelShaderByteCode->GetBufferSize(), NULL, _pixelShader.GetAddressOf()));
}

void TerrainNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ThrowIfFailed(_device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
}

void TerrainNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void TerrainNode::BuildRendererStates()
{
	// Set default and wireframe rasteriser states
	D3D11_RASTERIZER_DESC rasteriserDesc;
	rasteriserDesc.FillMode = D3D11_FILL_SOLID;
	rasteriserDesc.CullMode = D3D11_CULL_BACK;
	rasteriserDesc.FrontCounterClockwise = false;
	rasteriserDesc.DepthBias = 0;
	rasteriserDesc.SlopeScaledDepthBias = 0.0f;
	rasteriserDesc.DepthBiasClamp = 0.0f;
	rasteriserDesc.DepthClipEnable = true;
	rasteriserDesc.ScissorEnable = false;
	rasteriserDesc.MultisampleEnable = false;
	rasteriserDesc.AntialiasedLineEnable = false;
	ThrowIfFailed(_device->CreateRasterizerState(&rasteriserDesc, _defaultRasteriserState.GetAddressOf()));
	rasteriserDesc.FillMode = D3D11_FILL_WIREFRAME;
	ThrowIfFailed(_device->CreateRasterizerState(&rasteriserDesc, _wireframeRasteriserState.GetAddressOf()));
}

bool TerrainNode::LoadHeightMap(wstring heightMapFilename)
{
	unsigned int mapSize = _numberOfXPoints * _numberOfZPoints;
	USHORT* rawFileValues = new USHORT[mapSize];

	std::ifstream inputHeightMap;
	inputHeightMap.open(heightMapFilename.c_str(), std::ios_base::binary);
	if (!inputHeightMap)
	{
		return false;
	}

	inputHeightMap.read((char*)rawFileValues, mapSize * 2);
	inputHeightMap.close();

	// Normalise BYTE values to the range 0.0f - 1.0f;
	for (unsigned int i = 0; i < mapSize; i++)
	{
		_heightValues.push_back((float)rawFileValues[i] / 65536);
	}
	delete[] rawFileValues;
	return true;
}

void TerrainNode::LoadTerrainTextures()
{
	wstring terrainTextureNames[5] = { L"grass.dds", L"darkdirt.dds", L"stone.dds", L"lightdirt.dds", L"snow.dds" };

	// Load the textures from the files
	ComPtr<ID3D11Resource> terrainTextures[5];
	for (int i = 0; i < 5; i++)
	{
		ThrowIfFailed(CreateDDSTextureFromFileEx(_device.Get(),
			_deviceContext.Get(),
			terrainTextureNames[i].c_str(),
			0,
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			false,
			terrainTextures[i].GetAddressOf(),
			nullptr
		));
	}
	// Now create the Texture2D arrary.  We assume all textures in the
	// array have the same format and dimensions

	D3D11_TEXTURE2D_DESC textureDescription;
	ComPtr<ID3D11Texture2D> textureInterface;
	terrainTextures[0].As<ID3D11Texture2D>(&textureInterface);
	textureInterface->GetDesc(&textureDescription);

	D3D11_TEXTURE2D_DESC textureArrayDescription;
	textureArrayDescription.Width = textureDescription.Width;
	textureArrayDescription.Height = textureDescription.Height;
	textureArrayDescription.MipLevels = textureDescription.MipLevels;
	textureArrayDescription.ArraySize = 5;
	textureArrayDescription.Format = textureDescription.Format;
	textureArrayDescription.SampleDesc.Count = 1;
	textureArrayDescription.SampleDesc.Quality = 0;
	textureArrayDescription.Usage = D3D11_USAGE_DEFAULT;
	textureArrayDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureArrayDescription.CPUAccessFlags = 0;
	textureArrayDescription.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> textureArray = 0;
	ThrowIfFailed(_device->CreateTexture2D(&textureArrayDescription, 0, textureArray.GetAddressOf()));

	// Copy individual texture elements into texture array.

	for (UINT i = 0; i < 5; i++)
	{
		// For each mipmap level...
		for (UINT mipLevel = 0; mipLevel < textureDescription.MipLevels; mipLevel++)
		{
			_deviceContext->CopySubresourceRegion(textureArray.Get(),
				D3D11CalcSubresource(mipLevel, i, textureDescription.MipLevels),
				NULL,
				NULL,
				NULL,
				terrainTextures[i].Get(),
				mipLevel,
				nullptr
			);
		}
	}

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDescription;
	viewDescription.Format = textureArrayDescription.Format;
	viewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDescription.Texture2DArray.MostDetailedMip = 0;
	viewDescription.Texture2DArray.MipLevels = textureArrayDescription.MipLevels;
	viewDescription.Texture2DArray.FirstArraySlice = 0;
	viewDescription.Texture2DArray.ArraySize = 5;

	ThrowIfFailed(_device->CreateShaderResourceView(textureArray.Get(), &viewDescription, _texturesResourceView.GetAddressOf()));
}

void TerrainNode::GenerateBlendMap()
{
	DWORD* blendMap = new DWORD[_numberOfRows * _numberOfColumns];
	DWORD* blendMapPtr = blendMap;
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;

	DWORD index = 0;
	for (DWORD i = 0; i < _numberOfRows; i++)
	{
		for (DWORD j = 0; j < _numberOfColumns; j++)
		{
			r = 0;
			g = 0;
			b = 0;
			a = 0;
			
			int waterLevel = static_cast<int>(_worldMaxHeight * 0.1f);
			int shoreLevel = static_cast<int>(_worldMaxHeight * 0.2f);
			int peakLevel = static_cast<int>(_worldMaxHeight * 0.6f);
			int mountainTopLevel = static_cast<int>(_worldMaxHeight * 0.7f);

			TerrainVertex currentTile = _vertices[4 * (i * _numberOfColumns + j)];

			if (currentTile.Position.y < waterLevel)
			{
				r = 255;
				b = r;
			}
			else if (currentTile.Position.y < shoreLevel)
			{
				r = static_cast<BYTE>((shoreLevel - currentTile.Position.y) * 255 / (shoreLevel - waterLevel));
				b = r;
			}
			else if (currentTile.Position.y > peakLevel && currentTile.Position.y < mountainTopLevel)
			{
				g = static_cast<BYTE>((currentTile.Position.y - peakLevel) * 255 / (mountainTopLevel - peakLevel));
			}
			else if (currentTile.Position.y >= mountainTopLevel)
			{
				g = 255;
				//a = static_cast<BYTE>((currentTile.Position.y - mountainTopLevel) * 255 / (_worldMaxHeight - mountainTopLevel));
			}
			
			DWORD mapValue = (a << 24) + (b << 16) + (g << 8) + r;
			*blendMapPtr++ = mapValue;
		}
	}
	D3D11_TEXTURE2D_DESC blendMapDescription;
	blendMapDescription.Width = _numberOfRows;
	blendMapDescription.Height = _numberOfColumns;
	blendMapDescription.MipLevels = 1;
	blendMapDescription.ArraySize = 1;
	blendMapDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	blendMapDescription.SampleDesc.Count = 1;
	blendMapDescription.SampleDesc.Quality = 0;
	blendMapDescription.Usage = D3D11_USAGE_DEFAULT;
	blendMapDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	blendMapDescription.CPUAccessFlags = 0;
	blendMapDescription.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA blendMapInitialisationData;
	blendMapInitialisationData.pSysMem = blendMap;
	blendMapInitialisationData.SysMemPitch = 4 * _numberOfColumns;

	ComPtr<ID3D11Texture2D> blendMapTexture;
	ThrowIfFailed(_device->CreateTexture2D(&blendMapDescription, &blendMapInitialisationData, blendMapTexture.GetAddressOf()));

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDescription;
	viewDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDescription.Texture2D.MostDetailedMip = 0;
	viewDescription.Texture2D.MipLevels = 1;

	ThrowIfFailed(_device->CreateShaderResourceView(blendMapTexture.Get(), &viewDescription, _blendMapResourceView.GetAddressOf()));
	delete[] blendMap;
}

float TerrainNode::GetHeightAtPoint(float x, float z)
{
	int cellX = (int)((x - _terrainStartX) / _spacing);
	int cellZ = (int)((_terrainStartZ - z) / _spacing);

	float dx = x - (_terrainStartX + cellX * _spacing);
	float dz = z - (_terrainStartZ - (cellZ + 1) * _spacing);

	int index = 4 * (cellZ * _numberOfColumns + cellX);
	if (dz > dx)
	{
		XMVECTOR v1 = XMLoadFloat3(&_vertices[index].Position);
		XMVECTOR v2 = XMLoadFloat3(&_vertices[index + 1].Position);
		XMVECTOR v3 = XMLoadFloat3(&_vertices[index + 2].Position);
		XMFLOAT3 normal;
		XMStoreFloat3(&normal, XMVector3Cross(v2 - v1, v3 - v1));

		return _vertices[index + 2].Position.y - ((normal.x * dx + normal.z * dz) / normal.y);
	}
	else
	{
		XMVECTOR v1 = XMLoadFloat3(&_vertices[index + 3].Position);
		XMVECTOR v2 = XMLoadFloat3(&_vertices[index + 2].Position);
		XMVECTOR v3 = XMLoadFloat3(&_vertices[index + 1].Position);
		XMFLOAT3 normal;
		XMStoreFloat3(&normal, XMVector3Cross(v2 - v1, v3 - v1));

		return _vertices[index + 2].Position.y - ((normal.x * dx + normal.z * dz) / normal.y);
	}
}

unsigned int TerrainNode::GetNumberOfXPoints()
{
	return _numberOfXPoints;
}

unsigned int TerrainNode::GetNumberOfZPoints()
{
	return _numberOfZPoints;
}

float TerrainNode::GetStartX()
{
	return _terrainStartX;
}

float TerrainNode::GetStartZ()
{
	return _terrainStartZ;
}

float TerrainNode::GetSpacing()
{
	return _spacing;
}
