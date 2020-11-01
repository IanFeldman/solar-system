#include "CollisionComponent.h"
#include "rigidbody.h"
#include <algorithm>
#include "Actor.h"

CollisionComponent::CollisionComponent(Actor* owner)
:Component(owner)
,mWidth(0.0f)
,mHeight(0.0f)
{
    mOwner = Component::mOwner;
}

CollisionComponent::~CollisionComponent()
{
	
}

bool CollisionComponent::Intersect(const CollisionComponent* other)
{
    bool case1 = GetMax().x < other->GetMin().x;
    bool case2 = other->GetMax().x < GetMin().x;
    bool case3 = GetMax().y < other->GetMin().y;
    bool case4 = other->GetMax().y < GetMin().y;
    if (!case1 && !case2 && !case3 && !case4) {
        return true;
    }
    else {
        return false;
    }
}

bool CollisionComponent::IntersectCircle(const CollisionComponent* other, float deltaTime, float& outT)
{
    float timeInterval = 10.0f;
    // GET PATH OF BOTH, CHECK IF INTERSECTS AT ANY POINT UP TO DELTATIME AT SOME INTERVAL
    for (float i = 0; i < deltaTime; i += deltaTime / timeInterval) {
        if (mRadius + other->GetRadius() > (mOwner->GetComponent<RigidBody>()->CalculatePath(i) - other->mOwner->GetComponent<RigidBody>()->CalculatePath(i)).Length()) {
            outT = i;
            return true;
        }
    }

    outT = -1.0f;
    return false;
}

Vector2 CollisionComponent::GetMin() const
{
	Vector2 min;
    min.x = mOwner->GetPosition().x - (mWidth * mOwner->GetScale()) / 2.0f;
    min.y = mOwner->GetPosition().y - (mHeight * mOwner->GetScale()) / 2.0f;
    return min;
}

Vector2 CollisionComponent::GetMax() const
{
	Vector2 max;
    max.x = mOwner->GetPosition().x + (mWidth * mOwner->GetScale()) / 2.0f;
    max.y = mOwner->GetPosition().y + (mHeight * mOwner->GetScale()) / 2.0f;
    return max;
}

const Vector2& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}