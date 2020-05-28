#pragma once
#include "SceneNode.h"
#include "DirectXFramework.h"
#include "SubMeshRenderer.h"

class SubMeshNode : public SceneNode
{
public:
	SubMeshNode(wstring name, shared_ptr<SubMesh> subMesh) : SceneNode(name) { _subMesh = subMesh; }

	bool Initialise();
	void Render();
	void Shutdown();

private:
	shared_ptr<SubMeshRenderer>		_renderer;

	shared_ptr<ResourceManager>		_resourceManager;
	shared_ptr<SubMesh>				_subMesh;
};

