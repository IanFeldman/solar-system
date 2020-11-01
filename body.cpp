#include "body.h"
#include "game.h"
#include "math.h"

Body::Body(Game* game)
	:Actor(game)
{
	mCollider = new CollisionComponent(this);
	mSpriteComponent = new SpriteComponent(this);
	mRB = new RigidBody(this);
}

void Body::OnProcessInput(const Uint8* keyboardState)
{

}

void Body::OnUpdate(float deltaTime)
{
}

void Body::OnSelect()
{
	mRB->CreateMarkers();
}

void Body::OnDeselect()
{

}