#include "Graphics2.h"
#include "SphereCollider.h"

Graphics2 app;

void Graphics2::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// This is where you add nodes to the scene graph
	SceneNodePointer skyBox = make_shared<SkyNode>(L"Sky", L"skymap.dds", 5000.0f);
	sceneGraph->Add(skyBox);

	shared_ptr<TerrainNode> grid = make_shared<TerrainNode>(L"Grid", L"Example_HeightMap.raw");
	sceneGraph->Add(grid);
	
	shared_ptr<DividedMeshNode> tree = make_shared<DividedMeshNode>(L"Tree", L"Tree\\firtree1.3ds", XMMatrixRotationX(XM_PIDIV2), sphere);
	sceneGraph->Add(tree);
	_collideable.push_back(tree);

	shared_ptr<DividedMeshNode> tree2 = make_shared<DividedMeshNode>(L"SecondTree", L"Tree\\firtree1.3ds", XMMatrixRotationX(XM_PIDIV2), sphere);
	sceneGraph->Add(tree2);
	_collideable.push_back(tree2);

	shared_ptr<DividedMeshNode> tree3 = make_shared<DividedMeshNode>(L"ThirdTree", L"Tree\\firtree1.3ds", XMMatrixRotationX(XM_PIDIV2), sphere);
	sceneGraph->Add(tree3);
	_collideable.push_back(tree3);

	shared_ptr<DividedMeshNode> tree4 = make_shared<DividedMeshNode>(L"ForthTree", L"Tree\\firtree1.3ds", XMMatrixRotationX(XM_PIDIV2), sphere);
	sceneGraph->Add(tree4);
	_collideable.push_back(tree4);

	shared_ptr<DividedMeshNode> tree5 = make_shared<DividedMeshNode>(L"FifthTree", L"Tree\\firtree1.3ds", XMMatrixRotationX(XM_PIDIV2), sphere);
	sceneGraph->Add(tree5);
	_collideable.push_back(tree5);
	
	shared_ptr<MoveableNode> plane = make_shared<MoveableNode>(L"Plane", L"Plane\\Bonanza.3ds", XMMatrixRotationZ(XM_PI) * XMMatrixRotationX(XM_PIDIV2), sphere);
	sceneGraph->Add(plane);
	SceneGraphPointer airscrew = make_shared<SceneGraph>(L"Airscrew");
	plane->Add(airscrew);
	airscrew->Add(sceneGraph->Find(L"Plane58"));
	airscrew->Add(sceneGraph->Find(L"Plane59"));
	airscrew->Add(sceneGraph->Find(L"Plane60"));
	airscrew->Add(sceneGraph->Find(L"Plane61"));
	plane->SetNodePosition(0.0f, 500.0f, 0.0f);
	_collideable.push_back(plane);
	_pickable.push_back(plane);
	
	shared_ptr<MoveableNode> plane2 = make_shared<MoveableNode>(L"SecondPlane", L"CamoPlane\\Me262A1a.3ds", XMMatrixRotationX(XM_PIDIV2), sphere);
	sceneGraph->Add(plane2);
	plane2->SetNodePosition(500.0f, 500.0f, 1000.0f);
	_collideable.push_back(plane2);
	_pickable.push_back(plane2);

	shared_ptr<MoveableNode> plane3 = make_shared<MoveableNode>(L"ThirdPlane", L"CamoPlane\\Me262A1a.3ds", XMMatrixRotationX(XM_PIDIV2), sphere);
	sceneGraph->Add(plane3);
	plane3->SetNodePosition(-500.0f, 500.0f, 1000.0f);
	plane3->SetTotalYaw(90.0f);
	_collideable.push_back(plane3);
	_pickable.push_back(plane3);
	
	GetCamera()->FollowNode(dynamic_pointer_cast<MoveableNode>(plane), 0.0f, 25.0f, -50.0f);
	_player = plane;
}

void Graphics2::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();
	
	// This is where you make any changes to the local world transformations to nodes
	// in the scene graph
	shared_ptr<TerrainNode> grid = dynamic_pointer_cast<TerrainNode>(sceneGraph->Find(L"Grid"));
	
	SceneNodePointer tree = sceneGraph->Find(L"Tree");
	int x = -200;
	int z = 400;
	tree->SetWorldTransform(XMMatrixScaling(30.0f, 30.0f, 30.0f) * XMMatrixTranslation(x, grid->GetHeightAtPoint(x, z), z));

	SceneNodePointer tree2 = sceneGraph->Find(L"SecondTree");
	x = -100;
	z = 200;
	tree2->SetWorldTransform(XMMatrixScaling(30.0f, 30.0f, 30.0f) * XMMatrixTranslation(x, grid->GetHeightAtPoint(x, z), z));

	SceneNodePointer tree3 = sceneGraph->Find(L"ThirdTree");
	x = 50;
	z = 700;
	tree3->SetWorldTransform(XMMatrixScaling(30.0f, 30.0f, 30.0f) * XMMatrixTranslation(x, grid->GetHeightAtPoint(x, z), z));

	SceneNodePointer tree4 = sceneGraph->Find(L"ForthTree");
	x = 400;
	z = 100;
	tree4->SetWorldTransform(XMMatrixScaling(30.0f, 30.0f, 30.0f) * XMMatrixTranslation(x, grid->GetHeightAtPoint(x, z), z));

	SceneNodePointer tree5 = sceneGraph->Find(L"FifthTree");
	x = 225;
	z = 70;
	tree5->SetWorldTransform(XMMatrixScaling(30.0f, 30.0f, 30.0f) * XMMatrixTranslation(x, grid->GetHeightAtPoint(x, z), z));
	
	shared_ptr<MoveableNode> plane = dynamic_pointer_cast<MoveableNode>(sceneGraph->Find(L"Plane"));
	plane->SetWorldTransform(XMMatrixRotationX(XM_PIDIV2) * XMMatrixRotationY(XM_PI) * XMMatrixTranslation(0.0f, -14.0f, 0.0f));
	SceneNodePointer airscrew = sceneGraph->Find(L"Airscrew");
	airscrew->SetWorldTransform(XMMatrixTranslation(0.0f, -14.5f, 15.471f) * XMMatrixRotationZ(XMConvertToRadians(angle)) * XMMatrixTranslation(0.0f, 14.5f, -15.471f));
	angle += 10.0f;
	if (_player != plane && _player != nullptr)
	{
		plane->SetForwardBack(1);
		plane->SetYaw(2);
	}
	
	shared_ptr<MoveableNode> plane2 = dynamic_pointer_cast<MoveableNode>(sceneGraph->Find(L"SecondPlane"));
	if (_player != plane2 && _player != nullptr)
	{
		plane2->SetForwardBack(1);
		plane2->SetYaw(2);
	}
	
	shared_ptr<MoveableNode> plane3 = dynamic_pointer_cast<MoveableNode>(sceneGraph->Find(L"ThirdPlane"));
	if (_player != plane3 && _player != nullptr)
	{
		plane3->SetForwardBack(1);
		plane3->SetYaw(2);
	}
	
	if (_player)
	{
		_player->SetForwardBack(1);

		for (size_t i = 0; i < _collideable.size(); i++)
		{
			if (_player->IsColliding(_collideable[i]) && _collideable[i] != _player)
			{
				_player->SetForwardBack(-50);
			}
		}

		if (GetAsyncKeyState(0x57) < 0)
		{
			_player->SetPitch(-1);
		}
		if (GetAsyncKeyState(0x53) < 0)
		{
			_player->SetPitch(1);
		}
		if (GetAsyncKeyState(0x41) < 0)
		{
			_player->SetYaw(-1);
		}
		if (GetAsyncKeyState(0x44) < 0)
		{
			_player->SetYaw(1);
		}
		if (GetAsyncKeyState(0x51) < 0)
		{
			_player->SetRoll(1);
		}
		if (GetAsyncKeyState(0x45) < 0)
		{
			_player->SetRoll(-1);
		}
	}
	else
	{
		if (GetAsyncKeyState(0x57) < 0)
		{
			_player->SetPitch(-1);
		}
		if (GetAsyncKeyState(0x53) < 0)
		{
			_player->SetPitch(1);
		}
		if (GetAsyncKeyState(0x41) < 0)
		{
			_player->SetYaw(-1);
		}
		if (GetAsyncKeyState(0x44) < 0)
		{
			_player->SetYaw(1);
		}
		if (GetAsyncKeyState(0x51) < 0)
		{
			_player->SetRoll(1);
		}
		if (GetAsyncKeyState(0x45) < 0)
		{
			_player->SetRoll(-1);
		}

		/*POINT p;
		if (ScreenToClient(DirectXFramework::GetDXFramework()->GetHWnd(), &p))
		{
			XMFLOAT4X4 projectionMatrix;
			XMStoreFloat4x4(&projectionMatrix, GetProjectionTransformation());
			// Compute picking ray in view space.
			float vx = (+2.0f * p.x / GetWindowWidth() - 1.0f) /
				projectionMatrix(0, 0);
			float vy = (-2.0f * p.y / GetWindowHeight() + 1.0f) /
				projectionMatrix(1, 1);
			// Ray definition in view space.
			XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			XMVECTOR rayDirection = XMVectorSet(vx, vy, 1.0f, 0.0f);

			XMMATRIX viewTransformation = GetCamera()->GetViewMatrix();
			XMMATRIX inverseViewTransformation = XMMatrixInverse(&XMMatrixDeterminant(viewTransformation), viewTransformation);

			rayDirection = XMVector3Transform(rayDirection, inverseViewTransformation);

			float distance;
			for (size_t i = 0; i < _pickable.size(); i++)
			{
				dynamic_pointer_cast<SphereCollider>(_pickable[i]->GetBoundingVolume())->GetBoundingSphere().Intersects(rayOrigin, rayDirection, &distance);
			}
			Intersects(XMVECTOR, XMVECTOR, float&)
		}*/
	}

	if (GetAsyncKeyState(VK_SPACE) < 0)
	{
		GetCamera()->FreeCam();
		_player = nullptr;
	}

	XMFLOAT3 playerPosition;
	XMStoreFloat3(&playerPosition, plane->GetNodePosition());
	float minHeight = grid->GetHeightAtPoint(playerPosition.x, playerPosition.z);
	if (playerPosition.y <= minHeight + 10)
	{
		plane->SetNodePosition(playerPosition.x, minHeight + 10, playerPosition.z);
	}

	gamePad.ProcessGameController();
}
