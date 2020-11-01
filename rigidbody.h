#pragma once
#include "game.h"
#include "Component.h"
#include <Windows.h>
#include <set>
#include <map>
#include <vector>
#include "math.h"
#include "SDL2/SDL_stdinc.h"
#include "body.h"
#include "trajectorymarker.h"
#include <functional>

enum class RigidBodyState
{
    Enabled,
    Disabled
};

class RigidBody : public Component
{
public:
    RigidBody(class Actor* owner);
    void Update(float deltaTime) override;
    void PhysicsUpdate(float deltaTime);
    void UpdatePath(Vector2 initialPos, Vector2 initialVel);
    void UpdateTrajectory();
    Vector2 CalculatePath(float time) { return mPath(time); }
    Vector2 CalcAccel(Vector2 currPos);
    void AddCollidedWith(class Actor* actor) { mCollidedWith.emplace(actor); }
    void CheckElasticCollisions();
    void CheckElasticCollision(class Actor*);
    void SetPathTime(float time) { mPathTime = time; }
    float GetPathTime() { return mPathTime; }
    void SetInitialPathPosition(Vector2 pos) { mInitPathPos = pos; }
    void SetMass(float mass) { mMass = mass; }
    float GetMass() { return mMass; }
    void SetInitVel(Vector2 initVel) { mInitVel = initVel; }
    void SetState(RigidBodyState state) { mRBState = state; }
    RigidBodyState GetState() { return mRBState; }
    void SetNode(Node* node) { mNode = node; }
    void ClearMarkers() { mMarkers.clear(); mMarkersHidden = true; }
    void CreateMarkers(void);

private:
    RigidBodyState mRBState = RigidBodyState::Disabled;
    float mMass = 1000.0f;
    Vector2 mVel = Vector2::Zero;
    Vector2 mInitVel = Vector2::Zero;
    Vector2 mAccel = Vector2::Zero;
    bool mInitialized = false;
    bool mMarkersHidden = false;
    std::vector<TrajectoryMarker*> mMarkers;
    Vector2 mEndPos; // for collisions
    float mBallRadius;
    
    // checking collider
    std::set<class Actor*> mCollidedWith;
    Node* mNode; // the node it belongs to

    // path
    Vector2 mInitPathPos;
    float mPathTime = 0.0f;
    std::function<Vector2(float)> mPath;
};