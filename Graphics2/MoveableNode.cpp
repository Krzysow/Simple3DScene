#include "MoveableNode.h"

XMVECTOR defaultNodeForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR defaultNodeRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR defaultNodeUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

MoveableNode::MoveableNode(wstring name, wstring modelName, XMMATRIX orientation, ColliderType colliderType) : DividedMeshNode(name, modelName, orientation, colliderType)
{
	_nodePosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	_moveLeftRight = 0.0f;
	_moveUpDown = 0.0f;
	_moveForwardBack = 0.0f;
	_nodeYaw = 0.0f;
	_nodePitch = 0.0f;
	_nodeRoll = 0.0f;
}

MoveableNode::~MoveableNode()
{
}

void MoveableNode::Update(FXMMATRIX& currentWorldTransformation)
{
	XMVECTOR nodePosition;
	XMVECTOR nodeRight;
	XMVECTOR nodeForward;
	XMVECTOR nodeUp;

	// Yaw (rotation around the Y axis) will have an impact on the forward and right vectors
	XMMATRIX nodeRotationYaw = XMMatrixRotationAxis(defaultNodeUp, _nodeYaw);
	nodeRight = XMVector3TransformCoord(defaultNodeRight, nodeRotationYaw);
	nodeForward = XMVector3TransformCoord(defaultNodeForward, nodeRotationYaw);

	// Pitch (rotation around the X axis) impact the up and forward vectors
	XMMATRIX nodeRotationPitch = XMMatrixRotationAxis(nodeRight, _nodePitch);
	nodeUp = XMVector3TransformCoord(defaultNodeUp, nodeRotationPitch);
	nodeForward = XMVector3TransformCoord(nodeForward, nodeRotationPitch);

	// Roll (rotation around the Z axis) will impact the Up and Right vectors
	XMMATRIX nodeRotationRoll = XMMatrixRotationAxis(nodeForward, _nodeRoll);
	nodeUp = XMVector3TransformCoord(nodeUp, nodeRotationRoll);
	nodeRight = XMVector3TransformCoord(nodeRight, nodeRotationRoll);

	// Adjust the node position by the appropriate amount forward/back and left/right
	nodePosition = XMLoadFloat4(&_nodePosition) + _moveLeftRight * nodeRight + _moveForwardBack * nodeForward + _moveUpDown * nodeUp;
	XMStoreFloat4(&_nodePosition, nodePosition);

	// World transformation
	XMStoreFloat4x4(&_worldTransformation, nodeRotationYaw * nodeRotationPitch * nodeRotationRoll * XMMatrixTranslationFromVector(nodePosition));

	// Reset the amount we are moving
	_moveLeftRight = 0.0f;
	_moveUpDown = 0.0f;
	_moveForwardBack = 0.0f;
	
	SceneGraph::Update(currentWorldTransformation);
}

bool MoveableNode::IsColliding(shared_ptr<SceneNode> otherNode)
{
	return GetBoundingVolume()->IsIntersecting(otherNode->GetBoundingVolume());
}

XMVECTOR MoveableNode::GetNodePosition(void)
{
	return XMLoadFloat4(&_nodePosition);
}

void MoveableNode::SetNodePosition(float x, float y, float z)
{
	_nodePosition = XMFLOAT4(x, y, z, 0);
}

void MoveableNode::SetLeftRight(float leftRight)
{
	_moveLeftRight = leftRight;
}

void MoveableNode::SetUpDown(float upDown)
{
	_moveUpDown = upDown;
}

void MoveableNode::SetForwardBack(float forwardBack)
{
	_moveForwardBack = forwardBack;
}

void MoveableNode::SetPitch(float pitch)
{
	_nodePitch += XMConvertToRadians(pitch);
}

void MoveableNode::SetTotalPitch(float pitch)
{
	_nodePitch = XMConvertToRadians(pitch);
}

float MoveableNode::GetPitch() const
{
	return XMConvertToDegrees(_nodePitch);
}

void MoveableNode::SetYaw(float yaw)
{
	_nodeYaw += XMConvertToRadians(yaw);
}

void MoveableNode::SetTotalYaw(float yaw)
{
	_nodeYaw = XMConvertToRadians(yaw);
}

float MoveableNode::GetYaw() const
{
	return XMConvertToDegrees(_nodeYaw);
}

void MoveableNode::SetRoll(float roll)
{
	_nodeRoll += XMConvertToRadians(roll);
}

void MoveableNode::SetTotalRoll(float roll)
{
	_nodeRoll = XMConvertToRadians(roll);
}

float MoveableNode::GetRoll() const
{
	return XMConvertToDegrees(_nodeRoll);
}
