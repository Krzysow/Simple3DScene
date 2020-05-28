#pragma once
#include "Renderer.h"
#include "Mesh.h"

class SubMeshRenderer : public Renderer
{
public:
	void SetSubMesh(shared_ptr<SubMesh> subMesh);
	void SetWorldTransformation(FXMMATRIX worldTransformation);
	void SetAmbientLight(XMFLOAT4 ambientLight);
	void SetDirectionalLight(FXMVECTOR lightVector, XMFLOAT4 lightColour);
	void SetCameraPosition(XMFLOAT4 cameraPosition);
	void RenderTransparentNodes(bool renderTransparent);

	bool Initialise();
	void Render();
	void Shutdown(void);

private:
	shared_ptr<SubMesh>	_subMesh;
	XMFLOAT4X4			_worldTransformation;
	XMFLOAT4			_ambientLight;
	XMFLOAT4			_directionalLightVector;
	XMFLOAT4			_directionalLightColour;
	XMFLOAT4			_cameraPosition;
	bool				_renderTransparent;

	ComPtr<ID3D11Device>			_device;
	ComPtr<ID3D11DeviceContext>		_deviceContext;

	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;

	ComPtr<ID3DBlob>				_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;
	ComPtr<ID3D11InputLayout>		_layout;
	ComPtr<ID3D11Buffer>			_constantBuffer;

	ComPtr<ID3D11ShaderResourceView> _texture;;

	ComPtr<ID3D11BlendState>		 _transparentBlendState;

	ComPtr<ID3D11RasterizerState>    _defaultRasteriserState;
	ComPtr<ID3D11RasterizerState>    _noCullRasteriserState;

	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildBlendState();
	void BuildRendererState();
};

