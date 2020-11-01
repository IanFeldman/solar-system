#pragma once
#include "actor.h"
#include "spritecomponent.h"

class SliderKnob : public Actor
{
public:
	SliderKnob(class Game* game);
	void OnUpdate(float deltaTime) override;
	void OnProcessInput(const Uint8* keyboardState) override;

private:
	class SpriteComponent* mSpriteComponent;
};