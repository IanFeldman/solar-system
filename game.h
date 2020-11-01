#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "SDL2/SDL.h"
#include "math.h"
#include "spritecomponent.h"

struct Camera
{
    Vector2 position;
    float rotation;
    float scale;
};

struct Node
{
    Vector2 position;
    std::vector<Node*> nearbyNodes;
    std::vector<Actor*> actors;
    float radius = 512.0f * 0.6f;
};

class Game {

public:
    Game();
    bool Initialize(void);
    void Shutdown(void);
    void Runloop(void);
    void ProcessUpdate(void);
    void UpdateGame(void);
    void GenerateOutput(void);
    void LoadData(void);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);
    void AddMarker(class TrajectoryMarker* tMarker) { mAllMarkers.push_back(tMarker); }
    void DestroyMarkers(void);
    std::vector<class TrajectoryMarker*> GetMarkers() { return mAllMarkers; }
    std::vector<class Actor*> GetActors() { return mActors; }
    void AddBody(class Body* body) { mBodies.push_back(body); }
    void AddSprite(SpriteComponent* sprite);
    void RemoveSprite(SpriteComponent* sprite);
    SDL_Texture* GetTexture(const char* fileName);
    Vector2 GetWindowSize() { return mWindowSize; }
    Camera* GetCamera() { return mCamera; }
    float GetGravitationalConstant() { return mGravitationalConstant; }
    std::vector<class Body*> GetBodies() { return mBodies; }
    std::vector<Node*> GetNodes() { return mAllNodes; }
    void DebugStats();

private:
    void UnloadData(void);

protected:
    Camera* mCamera;
    std::vector<class Body*> mBodies;
    std::vector<Node*> mAllNodes;
    std::vector<std::vector<Node*>> mNodeGrid;
    std::pair<int, int> mGridSize = std::make_pair(24, 24);
    float mGravitationalConstant = 0.01f;
    bool mRunning;
    int mPreviousTime;
    class Selector* mSelector;
    class InitPlanetUI* mInitPanel;
    Vector2 mWindowSize = Vector2::Zero;
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    std::vector<class Actor*> mActors;
    std::unordered_map<std::string, SDL_Texture*> mTextureCache;
    std::vector<SpriteComponent*> mSprites;
    std::vector<class TrajectoryMarker*> mAllMarkers;
};