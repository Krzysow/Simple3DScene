#include "SubMeshNode.h"

bool SubMeshNode::Initialise()
{
	_resourceManager = DirectXFramework::GetDXFramework()->GetResourceManager();
	_renderer = dynamic_pointer_cast<SubMeshRenderer>(_resourceManager->GetRenderer(L"SUB"));
	return _renderer->Initialise();
}

void SubMeshNode::Shutdown()
{

}

void SubMeshNode::Render()
{
	_renderer->SetSubMesh(_subMesh);
	_renderer->SetWorldTransformation(XMLoadFloat4x4(&_combinedWorldTransformation));
	XMFLOAT4 cameraPosition;
	XMStoreFloat4(&cameraPosition, DirectXFramework::GetDXFramework()->GetCamera()->GetCameraPosition());
	_renderer->SetCameraPosition(cameraPosition);
	_renderer->SetAmbientLight(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	_renderer->SetDirectionalLight(XMVectorSet(-1.0f, -1.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	_renderer->Render();
}
