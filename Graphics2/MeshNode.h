#pragma once
#include "SceneNode.h"
#include "DirectXFramework.h"
#include "MeshRenderer.h"

class MeshNode : public SceneNode
{
public:
	MeshNode(wstring name, wstring modelName, XMMATRIX orientation, ColliderType colliderType) : SceneNode(name) { _modelName = modelName; _orientation = orientation; _colliderType = colliderType; }

	bool Initialise();
	void Render();
	void Shutdown();

private:
	shared_ptr<MeshRenderer>		_renderer;

	wstring							_modelName;
	shared_ptr<ResourceManager>		_resourceManager;
	shared_ptr<Mesh>				_mesh;

	XMMATRIX						_orientation;
	ColliderType					_colliderType;
};

