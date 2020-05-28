#include "SphereCollider.h"

SphereCollider::SphereCollider(VERTEX* vertices, size_t numberOfVertices)
{
	XMFLOAT3* arr = new XMFLOAT3[numberOfVertices];
	for (size_t i = 0; i < numberOfVertices; ++i)
	{
		arr[i] = vertices[i].Position;
	}
	BoundingSphere::CreateFromPoints(_boundingSphere, numberOfVertices, arr, 0);
	delete[] arr;

	_localCenter = _boundingSphere.Center;
}

SphereCollider::SphereCollider(BoundingSphere source)
{
	_boundingSphere = source;
	_localCenter = _boundingSphere.Center;
	_localRadius = _boundingSphere.Radius;
}

SphereCollider::~SphereCollider()
{
}

bool SphereCollider::IsIntersecting(shared_ptr<BoundingVolume> otherVolume)
{
	//switch ()
	return _boundingSphere.Intersects(dynamic_pointer_cast<SphereCollider>(otherVolume)->GetBoundingSphere());
}

void SphereCollider::Update(FXMMATRIX& worldTransformation)
{
	_boundingSphere.Center = _localCenter;
	_boundingSphere.Radius = _localRadius;
	_boundingSphere.Transform(_boundingSphere, worldTransformation);

}

BoundingSphere SphereCollider::GetBoundingSphere()
{
	return _boundingSphere;
}
