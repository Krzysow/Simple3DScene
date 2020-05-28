#pragma once
#include "BoundingVolume.h"
#include "directxcollision.h"
#include "ResourceManager.h"

class SphereCollider : public BoundingVolume
{
public:
	SphereCollider(VERTEX* vertices, size_t numberOfVertices);
	SphereCollider(BoundingSphere source);
	~SphereCollider();

	bool IsIntersecting(shared_ptr<BoundingVolume> otherVolume);
	void Update(FXMMATRIX& worldTransformation);

	BoundingSphere GetBoundingSphere();

private:
	BoundingSphere _boundingSphere;
	XMFLOAT3 _localCenter;
	float _localRadius;
};
