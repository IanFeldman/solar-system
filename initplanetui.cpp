#include "initplanetui.h"
#include "sliderknob.h"
#include "game.h"
#include "math.h"

InitPlanetUI::InitPlanetUI(Game* game)
	:Actor(game)
{
	SetScale(5.0f);
	mSpriteComponent = new SpriteComponent(this, 105);
	mSpriteComponent->SetTexture(GetGame()->GetTexture("assets/init-panel.png"));
	mSpriteComponent->SetIsUI(true);

	Vector2 pos = Vector2(mGame->GetWindowSize().x / 5.0f, mGame->GetWindowSize().y * 0.55f);
	SetPosition(pos);

	Vector2 offset;
	mMassSlider = new SliderKnob(mGame);
	offset = Vector2(-102, -60);
	mMassSlider->SetPosition(mPosition + offset);
	mKnobs.push_back(mMassSlider);

	mVelocityXSlider = new SliderKnob(mGame);
	offset = Vector2(2, 45);
	mVelocityXSlider->SetPosition(mPosition + offset);
	mKnobs.push_back(mVelocityXSlider);

	mVelocityYSlider = new SliderKnob(mGame);
	offset = Vector2(2, 90);
	mVelocityYSlider->SetPosition(mPosition + offset);
	mKnobs.push_back(mVelocityYSlider);

	mSizeSlider = new SliderKnob(mGame);
	offset = Vector2(-102, 190);
	mSizeSlider->SetPosition(mPosition + offset);
	mKnobs.push_back(mSizeSlider);

	HideSliders();
}

void InitPlanetUI::HideSliders()
{
	for (SliderKnob* knob : mKnobs)
	{
		knob->GetComponent<SpriteComponent>()->SetIsVisible(false);
	}
}

void InitPlanetUI::ShowSliders()
{
	for (SliderKnob* knob : mKnobs)
	{
		knob->GetComponent<SpriteComponent>()->SetIsVisible(true);
	}
}

void InitPlanetUI::OnUpdate(float deltaTime)
{
}