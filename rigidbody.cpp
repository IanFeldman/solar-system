#include "RigidBody.h"
#include "Actor.h"
#include "Game.h"
#include <Windows.h>
#include <iostream>

RigidBody::RigidBody(Actor* owner)
    :Component(owner)
{
    CreateMarkers();
    auto Path = [this](float time) -> Vector2 { return mOwner->GetPosition() * mVel * time; };
    mPath = Path;

    float minDist = INT32_MAX;
    Node* closestNode = nullptr;
    for (Node* n : mOwner->GetGame()->GetNodes())
    {
        if ((mOwner->GetPosition() - n->position).Length() < minDist)
        {
            minDist = (mOwner->GetPosition() - n->position).Length();
            closestNode = n;
        }
    }
    mNode = closestNode;
    mNode->actors.push_back(mOwner);
}

void RigidBody::Update(float deltaTime)
{
    // This updates once on the first update loop and then never again
    if (!mInitialized)
    {
        mVel = mInitVel;
        mBallRadius = 32.0f * mOwner->GetScale();
        mInitialized = true;
    }
    if ((mOwner->GetPosition() - mNode->position).Length() > mNode->radius)
    {
        auto it = std::find(mNode->actors.begin(), mNode->actors.end(), mOwner);
        mNode->actors.erase(it);
        float minDist = INT32_MAX;
        Node* closestNode = nullptr;
        for (Node* n : mNode->nearbyNodes)
        {
            if ((mOwner->GetPosition() - n->position).Length() < minDist)
            {
                minDist = (mOwner->GetPosition() - n->position).Length();
                closestNode = n;
            }
        }
        if (closestNode != nullptr)
        {
            mNode = closestNode;
            mNode->actors.push_back(mOwner);
        }
    }
}

void RigidBody::UpdateTrajectory()
{
    if (!mMarkersHidden)
    {
        // calculate trajectory
        Vector2 vel = mVel;
        Vector2 accel = Vector2::Zero;
        Vector2 pos = mOwner->GetPosition();
        float interval = 0.1f;

        for (int i = 0; i < mMarkers.size(); i++)
        {
            // find accel
            accel = CalcAccel(pos);
            // set new vel
            vel = interval * accel + vel;
            // set new position
            pos = (interval * vel + pos);
            //leave marker
            mMarkers[i]->SetPosition(pos);
        }

        mMarkersHidden = true;
    }
}

void RigidBody::PhysicsUpdate(float deltaTime)
{
    mAccel = CalcAccel(mOwner->GetPosition());

    mInitPathPos = mOwner->GetPosition();
    mPathTime = deltaTime;
    UpdatePath(mInitPathPos, mVel);

    // mOwner->SetRotation((deltaTime * GetAngularSpeed()) + mOwner->GetRotation());
}

void RigidBody::UpdatePath(Vector2 initialPos, Vector2 initialVel)
{
    // not sure whether we should set mvel here or not but it's currently working
    auto Path = [this, initialVel, initialPos](float time) -> Vector2 { mVel = initialVel + mAccel * time; Vector2 pos = initialPos + mVel * time; return pos; };
    mPath = Path;
}

void RigidBody::CheckElasticCollisions()
{
    Actor* closestObject = nullptr;
    float shortestTimeInterval = mPathTime;
    std::vector<class Actor*> nearbyActors;
    for (class Actor* a : mNode->actors)
    {
        nearbyActors.push_back(a);
    }
    for (Node* n : mNode->nearbyNodes)
    {
        for (Actor* a : n->actors)
        {
            nearbyActors.push_back(a);
        }
    }

    for (Actor* object : nearbyActors) {
        if (object != mOwner && object->GetComponent<RigidBody>()->GetState() != RigidBodyState::Disabled && std::find(mCollidedWith.begin(), mCollidedWith.end(), object) == mCollidedWith.end()) {
            CollisionComponent* thisCC = mOwner->GetComponent<CollisionComponent>();
            CollisionComponent* otherCC = object->GetComponent<CollisionComponent>();
            float outTime;
            if (thisCC->IntersectCircle(otherCC, mPathTime, outTime)) {
                if (outTime < shortestTimeInterval) {
                    closestObject = object;
                    shortestTimeInterval = outTime;
                }
            }
        }
    }

    if (closestObject != nullptr) {
        CheckElasticCollision(closestObject);
    }

    mCollidedWith.clear();
}

void RigidBody::CheckElasticCollision(Actor* object)
{
    float outTime = 0.0f;
    CollisionComponent* thisCC = mOwner->GetComponent<CollisionComponent>();
    CollisionComponent* otherCC = object->GetComponent<CollisionComponent>();
    if (thisCC->IntersectCircle(otherCC, mPathTime, outTime)) {
        RigidBody* otherRB = object->GetComponent<RigidBody>();
        Vector2 thisCollisionPoint = CalculatePath(outTime);
        Vector2 otherCollisionPoint = otherRB->CalculatePath(outTime);

        mInitPathPos = thisCollisionPoint;
        otherRB->SetInitialPathPosition(otherCollisionPoint);
        mPathTime -= outTime;
        otherRB->SetPathTime(otherRB->GetPathTime() - outTime);

        Vector2 uNormal = Vector2::Normalize(otherCollisionPoint - thisCollisionPoint);
        Vector2 uTangent(-1 * uNormal.y, uNormal.x);
        Vector2 thisVel = mVel;
        Vector2 otherVel = otherRB->mVel;
        float thisVelNormalInit = Vector2::Dot(thisVel, uNormal);
        float thisVelTangentInit = Vector2::Dot(thisVel, uTangent);
        float otherVelNormalInit = Vector2::Dot(otherVel, uNormal);
        float otherVelTangentInit = Vector2::Dot(otherVel, uTangent);
        float thisVelNormalFinal = thisVelNormalInit * (mMass - otherRB->GetMass()) + 2.0f * otherRB->GetMass() * otherVelNormalInit;
        thisVelNormalFinal /= (mMass + otherRB->GetMass());
        float thisVelTangentFinal = thisVelTangentInit;
        float otherVelNormalFinal = otherVelNormalInit * (otherRB->GetMass() - mMass) + 2.0f * mMass * thisVelNormalInit;
        otherVelNormalFinal /= (mMass + otherRB->GetMass());
        float otherVelTangentFinal = otherVelTangentInit;

        thisVel = thisVelNormalFinal * uNormal + thisVelTangentFinal * uTangent;
        otherVel = otherVelNormalFinal * uNormal + otherVelTangentFinal * uTangent;

        UpdatePath(thisCollisionPoint, thisVel);
        otherRB->UpdatePath(otherCollisionPoint, otherVel);

        // adding the object collided with to the collided with vector
        AddCollidedWith(object);
        otherRB->AddCollidedWith(mOwner);
        CheckElasticCollisions();
    }
}

Vector2 RigidBody::CalcAccel(Vector2 currPos)
{
    Vector2 totalAccel = Vector2::Zero;

    for (class Body* other : mOwner->GetGame()->GetBodies())
    {
        RigidBody* otherRB = other->GetComponent<RigidBody>();

        if (otherRB != this)
        {
            float sqrtDist = sqrt((other->GetPosition() - currPos).Length());  // sqrt of distance
            Vector2 forceDir = Vector2::Normalize(other->GetPosition() - currPos);  // normalized direction
            Vector2 force = forceDir * mOwner->GetGame()->GetGravitationalConstant() * mMass * otherRB->GetMass(); // multiplying by const and masses
            force = Vector2(force.x / sqrtDist, force.y / sqrtDist); // diving by sqrt of distance
            Vector2 acceleration = Vector2(force.x / mMass, force.y / mMass); // dividing by mass to get accel
            totalAccel += acceleration;
        }
    }

    return totalAccel;
}

void RigidBody::CreateMarkers()
{
    for (int i = 0; i < 50; i++)
    {
        TrajectoryMarker* tMarker = new TrajectoryMarker(mOwner->GetGame());
        mMarkers.push_back(tMarker); // add it to rigid body vector
        mOwner->GetGame()->AddMarker(tMarker); // add it to master game vector
        mMarkersHidden = false;
    }
}