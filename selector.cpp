#include "selector.h"
#include "game.h"
#include "math.h"

Selector::Selector(Game* game)
	:Actor(game)
{
	mSpriteComponent = new SpriteComponent(this, 101);
	mSpriteComponent->SetTexture(GetGame()->GetTexture("assets/selector.png"));
	mSpriteComponent->SetIsUnscaleable(true);
}

void Selector::OnProcessInput(const Uint8* keyboardState)
{
}

void Selector::OnUpdate(float deltaTime)
{
	if (mTarget)
	{
		Vector2 targetPos = mTarget->GetPosition();
		SetPosition(targetPos);
		SetScale(mTarget->GetScale());

		SetRotation(GetRotation() + 0.005f);
	}
}