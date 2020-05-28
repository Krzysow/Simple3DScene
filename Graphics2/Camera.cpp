#include "Camera.h"
#include "MoveableNode.h"

XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

Camera::Camera()
{
	_cameraPosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	_moveLeftRight = 0.0f;
	_moveUpDown = 0.0f;
	_moveForwardBack = 0.0f;
	_cameraYaw = 0.0f;
	_cameraPitch = 0.0f;
	_cameraRoll = 0.0f;
	_targetNode = nullptr;
	_offset = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

Camera::~Camera()
{
}

void Camera::FollowNode(shared_ptr<MoveableNode> targetNode, float offsetX, float offsetY, float offsetZ)
{
	_targetNode = targetNode;
	_offset = XMFLOAT4(offsetX, offsetY, offsetZ, 0.0f);
}

void Camera::FreeCam()
{
	_targetNode = nullptr;
}

void Camera::SetPitch(float pitch)
{
    _cameraPitch += XMConvertToRadians(pitch);
}

void Camera::SetTotalPitch(float pitch)
{
	_cameraPitch = XMConvertToRadians(pitch);
}

float Camera::GetPitch() const
{
	return XMConvertToDegrees(_cameraPitch);
}

void Camera::SetYaw(float yaw)
{
    _cameraYaw += XMConvertToRadians(yaw);
}

void Camera::SetTotalYaw(float yaw)
{
	_cameraYaw = XMConvertToRadians(yaw);
}

float Camera::GetYaw() const
{
	return XMConvertToDegrees(_cameraYaw);
}

void Camera::SetRoll(float roll)
{
    _cameraRoll += XMConvertToRadians(roll);
}

void Camera::SetTotalRoll(float roll)
{
	_cameraRoll = XMConvertToRadians(roll);
}

float Camera::GetRoll() const
{
	return XMConvertToDegrees(_cameraRoll);
}

void Camera::SetLeftRight(float leftRight)
{
    _moveLeftRight = leftRight;
}

void Camera::SetUpDown(float upDown)
{
	_moveUpDown = upDown;
}

void Camera::SetForwardBack(float forwardBack)
{
    _moveForwardBack = forwardBack;
}

XMMATRIX Camera::GetViewMatrix(void)
{
    return XMLoadFloat4x4(&_viewMatrix);
}

XMVECTOR Camera::GetCameraPosition(void)
{
    return XMLoadFloat4(&_cameraPosition);
}

void Camera::SetCameraPosition(float x, float y, float z)
{
    _cameraPosition = XMFLOAT4(x, y, z, 0.0f);
}

void Camera::Update(void)
{
	XMVECTOR cameraPosition;
	XMVECTOR cameraTarget;
	XMVECTOR cameraRight;
	XMVECTOR cameraForward;
	XMVECTOR cameraUp;
	
	if (_targetNode)
	{
		XMStoreFloat4(&_cameraPosition, _targetNode->GetNodePosition());
		_moveLeftRight = _offset.x;
		_moveUpDown = _offset.y;
		_moveForwardBack = _offset.z;
		SetTotalYaw(_targetNode->GetYaw());
		SetTotalPitch(_targetNode->GetPitch());
		SetTotalRoll(_targetNode->GetRoll());
	}
	
	// Yaw (rotation around the Y axis) will have an impact on the forward and right vectors
	XMMATRIX cameraRotationYaw = XMMatrixRotationAxis(defaultUp, _cameraYaw);
	cameraRight = XMVector3TransformCoord(defaultRight, cameraRotationYaw);
	cameraForward = XMVector3TransformCoord(defaultForward, cameraRotationYaw);

	// Pitch (rotation around the X axis) impact the up and forward vectors
	XMMATRIX cameraRotationPitch = XMMatrixRotationAxis(cameraRight, _cameraPitch);
	cameraUp = XMVector3TransformCoord(defaultUp, cameraRotationPitch);
	cameraForward = XMVector3TransformCoord(cameraForward, cameraRotationPitch);

	// Roll (rotation around the Z axis) will impact the Up and Right vectors
	XMMATRIX cameraRotationRoll = XMMatrixRotationAxis(cameraForward, _cameraRoll);
	cameraUp = XMVector3TransformCoord(cameraUp, cameraRotationRoll);
	cameraRight = XMVector3TransformCoord(cameraRight, cameraRotationRoll);

	// Adjust the camera position by the appropriate amount forward/back and left/right
	cameraPosition = XMLoadFloat4(&_cameraPosition) + _moveLeftRight * cameraRight + _moveUpDown * cameraUp + _moveForwardBack * cameraForward;
	XMStoreFloat4(&_cameraPosition, cameraPosition);

	// Reset the amount we are moving
	_moveLeftRight = 0.0f;
	_moveUpDown = 0.0f;
	_moveForwardBack = 0.0f;

	// Calculate a vector that tells us the direction the camera is looking in
	cameraTarget = cameraPosition + XMVector3Normalize(cameraForward);
	
	// and calculate our view matrix
	XMStoreFloat4x4(&_viewMatrix, XMMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp));
}
