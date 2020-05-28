#include "DividedMeshNode.h"
#include "SubMeshNode.h"
#include "SphereCollider.h"

DividedMeshNode::DividedMeshNode(wstring name,wstring modelName, XMMATRIX orientation, ColliderType colliderType) : SceneGraph(name)
{
	_modelName = modelName;
	_orientation = orientation;
	_colliderType = colliderType;
	_resourceManager = DirectXFramework::GetDXFramework()->GetResourceManager();
	_renderer = dynamic_pointer_cast<SubMeshRenderer>(_resourceManager->GetRenderer(L"SUB"));
	_mesh = _resourceManager->GetMesh(_modelName, _orientation, _colliderType);
	
	unsigned int subMeshCount = static_cast<unsigned int>(_mesh->GetSubMeshCount());
	shared_ptr<SubMeshNode> node;

	for (unsigned int i = 0; i < subMeshCount; i++)
	{
		wstringstream subMeshNameStream;
		subMeshNameStream << _name << i;
		wstring subMeshName = subMeshNameStream.str();
		node = make_shared<SubMeshNode>(subMeshName, _mesh->GetSubMesh(i));
		Add(node);
	}

	switch (_colliderType)
	{
	case sphere:
		{
			BoundingSphere bigBoy = dynamic_pointer_cast<SphereCollider>(_mesh->GetSubMesh(0)->GetBoundingVolume())->GetBoundingSphere();
			for (size_t i = 1; i < subMeshCount; ++i)
			{
				BoundingSphere::CreateMerged(bigBoy, bigBoy, dynamic_pointer_cast<SphereCollider>(_mesh->GetSubMesh(i)->GetBoundingVolume())->GetBoundingSphere());
			}
			_boundingVolume = make_shared<SphereCollider>(bigBoy);
			break;
		}
	case box:
		{
			break;
		}
	}
}

DividedMeshNode::~DividedMeshNode()
{
}

bool DividedMeshNode::Initialise()
{
	_renderer->Initialise();
	_isInitialised = true;
	return SceneGraph::Initialise();
}

void DividedMeshNode::Render()
{
	if (_isInitialised)
	{
		_renderer->SetWorldTransformation(XMLoadFloat4x4(&_combinedWorldTransformation));
		XMFLOAT4 cameraPosition;
		XMStoreFloat4(&cameraPosition, DirectXFramework::GetDXFramework()->GetCamera()->GetCameraPosition());
		_renderer->SetCameraPosition(cameraPosition);
		_renderer->SetAmbientLight(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		_renderer->SetDirectionalLight(XMVectorSet(-1.0f, -1.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		_renderer->RenderTransparentNodes(false);
		SceneGraph::Render();
		_renderer->RenderTransparentNodes(true);
		SceneGraph::Render();
	}
}

void DividedMeshNode::Shutdown()
{
	_resourceManager->ReleaseMesh(_modelName);
}
