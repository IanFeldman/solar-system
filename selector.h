#pragma once
#include "actor.h"
#include "spritecomponent.h"
#include "body.h"

class Selector : public Actor
{
public:
	Selector(class Game* game);
	void OnUpdate(float deltaTime) override;
	void OnProcessInput(const Uint8* keyboardState) override;
	void SetTarget(class Body* body) { mTarget = body; }
	class Body* GetTarget(void) { return mTarget; }

private:
	class SpriteComponent* mSpriteComponent;
	class Body* mTarget;
};