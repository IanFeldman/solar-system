#include "sliderknob.h"
#include "game.h"
#include "math.h"

SliderKnob::SliderKnob(Game* game)
	:Actor(game)
{
	SetScale(4.0f);
	mSpriteComponent = new SpriteComponent(this, 110);
	mSpriteComponent->SetIsUI(true);
	mSpriteComponent->SetTexture(mGame->GetTexture("assets/slider-knob.png"));
}

void SliderKnob::OnProcessInput(const Uint8* keyboardState)
{

}

void SliderKnob::OnUpdate(float deltaTime)
{
}