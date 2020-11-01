#include "trajectorymarker.h"
#include "game.h"
#include "math.h"

TrajectoryMarker::TrajectoryMarker(Game* game)
	:Actor(game)
{
	SetState(ActorState::Paused);
	mSpriteComponent = new SpriteComponent(this, 1);
	mSpriteComponent->SetTexture(GetGame()->GetTexture("assets/marker.png"));
	mSpriteComponent->SetIsUnscaleable(true);
}

void TrajectoryMarker::OnProcessInput(const Uint8* keyboardState)
{

}

void TrajectoryMarker::OnUpdate(float deltaTime)
{
}