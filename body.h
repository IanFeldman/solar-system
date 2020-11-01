#pragma once
#include "actor.h"
#include "spritecomponent.h"
#include "collisioncomponent.h"
#include "rigidbody.h"

class Body : public Actor
{
public:
	Body(class Game* game);
	void OnUpdate(float deltaTime) override;
	void OnProcessInput(const Uint8* keyboardState) override;
	void OnSelect();
	void OnDeselect();

private:
	class CollisionComponent* mCollider;
	class SpriteComponent* mSpriteComponent;
	class RigidBody* mRB;
};