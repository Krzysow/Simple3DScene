#pragma once
#include "SceneNode.h"
#include "DirectXFramework.h"
#include "DDSTextureLoader.h"

struct Vertex
{
	XMFLOAT3 Position;
};

class SkyNode : public SceneNode
{
public:
	SkyNode(wstring name, const wstring textureFilename, float radius);
	~SkyNode();

	virtual bool Initialise();
	virtual void Render();
	virtual void Shutdown() {};

private:
	ComPtr<ID3D11Device>				_device = DirectXFramework::GetDXFramework()->GetDevice();
	ComPtr<ID3D11DeviceContext>			_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();

	ComPtr<ID3D11Buffer>				_vertexBuffer;
	unsigned int						_numberOfVertices;
	ComPtr<ID3D11Buffer>				_indexBuffer;
	unsigned int						_numberOfIndices;

	ComPtr<ID3DBlob>					_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>					_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>			_vertexShader;
	ComPtr<ID3D11PixelShader>			_pixelShader;
	ComPtr<ID3D11InputLayout>			_layout;
	ComPtr<ID3D11Buffer>				_constantBuffer;

	std::vector<Vertex>					_vertices;
	std::vector<UINT>					_indices;

	wstring								_textureFilename;
	ComPtr<ID3D11ShaderResourceView>	_texture;
	float								_radius;

	ComPtr<ID3D11RasterizerState>		_defaultRasteriserState;
	ComPtr<ID3D11RasterizerState>		_noCullRasteriserState;

	ComPtr<ID3D11DepthStencilState>		_stencilState;

	void BuildGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildTexture();
	void CreateSphere(float radius, size_t tessellation);
	void BuildRendererStates();
	void BuildDepthStencilState();
};
