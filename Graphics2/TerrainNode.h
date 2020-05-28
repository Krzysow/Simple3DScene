#pragma once
#include "SceneNode.h"
#include "DDSTextureLoader.h"
#include "DirectXFramework.h"
#include <fstream>

struct TerrainVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT2 BlendMapTexCoord;
};

class TerrainNode : public SceneNode
{
public:
	TerrainNode(wstring name, wstring heightMapFilename);
	~TerrainNode();

	virtual bool Initialise();
	virtual void Render();
	virtual void Shutdown() {};

	float GetHeightAtPoint(float x, float z);

	unsigned int GetNumberOfXPoints();
	unsigned int GetNumberOfZPoints();

	float GetStartX();
	float GetStartZ();

	float GetSpacing();

private:
	ComPtr<ID3D11Device>				_device = DirectXFramework::GetDXFramework()->GetDevice();
	ComPtr<ID3D11DeviceContext>			_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();

	ComPtr<ID3D11Buffer>				_vertexBuffer;
	ComPtr<ID3D11Buffer>				_indexBuffer;

	ComPtr<ID3DBlob>					_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>					_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>			_vertexShader;
	ComPtr<ID3D11PixelShader>			_pixelShader;
	ComPtr<ID3D11InputLayout>			_layout;
	ComPtr<ID3D11Buffer>				_constantBuffer;

	std::vector<TerrainVertex>			_vertices;
	std::vector<UINT>					_indices;
	std::vector<float>					_heightValues;

	wstring								_heightMapFilename;

	ComPtr<ID3D11RasterizerState>		_defaultRasteriserState;
	ComPtr<ID3D11RasterizerState>		_wireframeRasteriserState;

	ComPtr<ID3D11ShaderResourceView>	_texturesResourceView;
	ComPtr<ID3D11ShaderResourceView>	_blendMapResourceView;

	unsigned int						_numberOfXPoints = 1024;
	unsigned int						_numberOfZPoints = 1024;

	unsigned int						_numberOfColumns = _numberOfXPoints - 1;
	unsigned int						_numberOfRows = _numberOfZPoints - 1;

	float								_terrainStartX = -5120.0f;
	float								_terrainStartZ = 5110.0f;

	float								_spacing = 10.0f;
	int									_worldMaxHeight = 1024;

	void BuildGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildRendererStates();
	bool LoadHeightMap(wstring heightMapFilename);
	void LoadTerrainTextures();
	void GenerateBlendMap();
};
