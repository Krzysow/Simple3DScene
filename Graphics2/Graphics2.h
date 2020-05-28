#pragma once
#include "DirectXFramework.h"
#include "CubeNode.h"
#include "MeshNode.h"
#include "TerrainNode.h"
#include "SkyNode.h"
#include "MoveableNode.h"
#include "GamePadController.h"
#include <list>

class Graphics2 : public DirectXFramework
{
public:
	void CreateSceneGraph();
	void UpdateSceneGraph();

private:
	GamePadController gamePad;
	float angle = 1.0f;

	shared_ptr<MoveableNode> _player = nullptr;
	vector<shared_ptr<DividedMeshNode>> _collideable;
	vector<shared_ptr<MoveableNode>> _pickable;
};
