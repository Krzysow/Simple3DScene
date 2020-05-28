#pragma once
#include "SceneGraph.h"
#include "ResourceManager.h"
#include "SubMeshRenderer.h"
#include <sstream>

class DividedMeshNode : public SceneGraph
{
public:
	DividedMeshNode(wstring name, wstring modelName, XMMATRIX orientation, ColliderType colliderType);
	~DividedMeshNode();

	bool Initialise();
	void Render();
	void Shutdown();

protected:
	XMMATRIX						_orientation;
	ColliderType					_colliderType;

private:
	shared_ptr<SubMeshRenderer>		_renderer;

	wstring							_modelName;
	shared_ptr<ResourceManager>		_resourceManager;
	shared_ptr<Mesh>				_mesh;

	bool							_isInitialised = false;
};

