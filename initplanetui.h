#pragma once
#include "actor.h"
#include "spritecomponent.h"
#include "sliderknob.h"

class InitPlanetUI : public Actor
{
public:
	InitPlanetUI(class Game* game);
	void OnUpdate(float deltaTime) override;
	void HideSliders();
	void ShowSliders();

private:
	class SpriteComponent* mSpriteComponent;
	std::vector<class SliderKnob*> mKnobs;
	class SliderKnob* mMassSlider;
	class SliderKnob* mVelocityXSlider;
	class SliderKnob* mVelocityYSlider;
	class SliderKnob* mSizeSlider;
};