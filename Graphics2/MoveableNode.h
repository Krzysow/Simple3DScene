#pragma once
#include "DividedMeshNode.h"

class MoveableNode : public DividedMeshNode
{
public:
	MoveableNode(wstring name, wstring modelName, XMMATRIX orientation, ColliderType colliderType);
	~MoveableNode();

	void Update(FXMMATRIX& currentWorldTransformation);

	bool IsColliding(shared_ptr<SceneNode> otherNode);

	XMVECTOR GetNodePosition(void);
	void SetNodePosition(float x, float y, float z);

	void SetLeftRight(float leftRight);
	void SetUpDown(float upDown);
	void SetForwardBack(float forwardBack);

	void SetPitch(float pitch);
	void SetTotalPitch(float pitch);
	float GetPitch() const;
	void SetYaw(float yaw);
	void SetTotalYaw(float yaw);
	float GetYaw() const;
	void SetRoll(float roll);
	void SetTotalRoll(float roll);
	float GetRoll() const;

private:
	XMFLOAT4 _nodePosition;

	float _moveLeftRight;
	float _moveUpDown;
	float _moveForwardBack;

	float _nodePitch;
	float _nodeYaw;
	float _nodeRoll;
};
