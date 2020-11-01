#include"SDL2/SDL_image.h"
#include <chrono>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <WinDef.h>
#include <fstream>
#include <string>
#include <cstring>
#include "Game.h"
#include "Actor.h"
#include "Math.h"
#include "body.h"
#include "selector.h"
#include "initplanetui.h"

Game::Game()
    :mRunning(true)
{
}

bool Game::Initialize() {
    int initialized = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    if (initialized != 0) {
        return false;
    }
    SDL_DisplayMode displayMode;
    SDL_GetDesktopDisplayMode(0, &displayMode);
    mWindowSize = Vector2(displayMode.w, displayMode.h);
    mWindow = SDL_CreateWindow("Solar System", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, displayMode.w, displayMode.h, 0);
    if (mWindow == NULL) {
        return false;
    }
    SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int imgInitialized = IMG_Init(IMG_INIT_PNG);
    if ((imgInitialized & IMG_INIT_PNG) != IMG_INIT_PNG) {
        return false;
    }
    if (mRenderer == NULL) {
        return false;
    }

    SDL_Surface* icon = IMG_Load("assets/earth.png");
    SDL_SetWindowIcon(mWindow, icon);

    LoadData();

    mPreviousTime = SDL_GetTicks();

    return true;
};

void Game::Shutdown() {
    UnloadData();
    IMG_Quit();
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
};

void Game::Runloop() {
    while (mRunning) {
        ProcessUpdate();
        UpdateGame();
        GenerateOutput();
    }
};

void Game::ProcessUpdate() {
    SDL_Event event;

    // Screen to world
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    Vector2 mousePos(mouseX, mouseY);
    Matrix3 manipulatorMatrix = Matrix3::Identity;
    Matrix3 scaleMatrix = Matrix3::CreateScale(mCamera->scale);
    Matrix3 translateMatrix = Matrix3::CreateTranslation(Vector2(mCamera->position.x * mCamera->scale + mWindowSize.x / 2, mCamera->position.y * mCamera->scale + mWindowSize.y / 2));
    manipulatorMatrix = scaleMatrix * translateMatrix;
    manipulatorMatrix.Invert();
    mousePos = Vector2::Transform(mousePos, manipulatorMatrix);

    float moveSpeed = 5.0f / mCamera->scale;
    if (mouseX < 0.1f * mWindowSize.x)
        mCamera->position.x += moveSpeed;
    if (mouseX > 0.9f * mWindowSize.x)
        mCamera->position.x -= moveSpeed;
    if (mouseY < 0.1f * mWindowSize.y)
        mCamera->position.y += moveSpeed;
    if (mouseY > 0.9f * mWindowSize.y)
        mCamera->position.y -= moveSpeed;

    int keyPressed;
    SDL_GetKeyboardState(&keyPressed);

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            mRunning = false;
            break;

        case SDL_MOUSEBUTTONDOWN:
        {
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
            {
                // if mouse is hovering over ui
                Vector2 panelSize(64.0f * mInitPanel->GetScale(), 128.0f * mInitPanel->GetScale());
                Vector2 panelPos = mInitPanel->GetPosition();
                if (mouseX > panelPos.x - (panelSize.x / 2.0f) && mouseX < panelPos.x + (panelSize.x / 2.0f) && mouseY > panelPos.y - (panelSize.y / 2.0f) && mouseY < panelPos.y + (panelSize.y / 2.0f))
                {
                    std::cout << "Mouse over UI" << std::endl;
                }
                else
                {
                    DestroyMarkers();
                    // create planet
                    Body* planet = new Body(this);
                    planet->SetScale(1.0f);
                    planet->GetComponent<CollisionComponent>()->SetRadius(16.0f);
                    planet->GetComponent<SpriteComponent>()->SetTexture(GetTexture("assets/planet-blue.png"));
                    planet->SetPosition(mousePos);
                    planet->GetComponent<RigidBody>()->SetMass(3.0f);
                    planet->GetComponent<RigidBody>()->SetInitVel(Vector2(500, 0));
                    mBodies.push_back(planet);
                    // select it
                    mSelector->SetTarget(planet);
                    mSelector->GetComponent<SpriteComponent>()->SetIsVisible(true);
                    // enable initPlanet UI
                    mInitPanel->GetComponent<SpriteComponent>()->SetIsVisible(true);
                    mInitPanel->ShowSliders();
                }
                break;
            }
            case SDL_BUTTON_RIGHT:
            {
                DestroyMarkers();
                // find closest body
                float maxDist = INT32_MAX;
                Body* closestBody = mBodies[0];
                for (Body* b : mBodies)
                {
                    float dist = (b->GetPosition() - mousePos).Length();
                    if (dist < maxDist)
                    {
                        maxDist = dist;
                        closestBody = b;
                    }
                }
                // if mouse is really close
                if (maxDist < 50.0f)
                {
                    mSelector->SetTarget(closestBody);
                    mSelector->GetComponent<SpriteComponent>()->SetIsVisible(true);
                    mInitPanel->GetComponent<SpriteComponent>()->SetIsVisible(true);
                    mInitPanel->ShowSliders();
                    closestBody->OnSelect();
                }
                // otherwise deselect all
                else
                {
                    mSelector->SetTarget(nullptr);
                    mSelector->GetComponent<SpriteComponent>()->SetIsVisible(false);
                    mInitPanel->GetComponent<SpriteComponent>()->SetIsVisible(false);
                    mInitPanel->HideSliders();
                }
                break;
            }
            
            default:
                break;
            }
            break;
        }
        case SDL_MOUSEWHEEL:
        {
            float scaleFac = 0.75f;
            if (event.wheel.y > 0)
                mCamera->scale /= scaleFac;
            else
                mCamera->scale *= scaleFac;
        }
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_SPACE:
                for (Body* b : mBodies)
                {
                    RigidBody* rb = b->GetComponent<RigidBody>();
                    if (rb->GetState() == RigidBodyState::Disabled)
                        rb->SetState(RigidBodyState::Enabled);
                }
                break;

            case SDLK_RETURN:
                mCamera->position = Vector2::Zero;
                mCamera->scale = 1.0f;
                break;

            case SDLK_i:
                DebugStats();
                break;

            default:
                break;
            }
        default:
            break;
        }
    }

    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    if (keyboardState[SDL_SCANCODE_ESCAPE])
        mRunning = false;

    // processing input for all actors
    std::vector<Actor*> copyActors = mActors;
    for (Actor* a : copyActors) {
        a->ProcessInput(keyboardState);
    }
}

void Game::UpdateGame() {
    // Setting fps
    while ((int)SDL_GetTicks() < mPreviousTime + 16) {}
    int currentTime = SDL_GetTicks();
    float deltaTime = (float)(currentTime - mPreviousTime) / 1000;
    if (deltaTime > 0.033f) {
        deltaTime = 0.033f;
    }
    mPreviousTime = currentTime;

    // call update on all actors
    std::vector<Actor*> actors = mActors;
    for (Actor* a : actors) {
        a->Update(deltaTime);
    }

    // destroy actors
    std::vector<Actor*> tempActors;
    for (Actor* a : mActors) {
        if (a->GetState() == ActorState::Destroy) {
            tempActors.emplace_back(a);
        }
    }
    if (!tempActors.empty()) {
        for (Actor* tempActor : tempActors) {
            delete tempActor;
        }
    }

    //  update physics
    for (Body* b : mBodies)
    {
        if (b->GetComponent<RigidBody>()->GetState() == RigidBodyState::Enabled)
            b->GetComponent<RigidBody>()->PhysicsUpdate(deltaTime);
    }

    // check collisions
    for (Body* b : mBodies)
    {
        if (b->GetComponent<RigidBody>()->GetState() == RigidBodyState::Enabled)
            b->GetComponent<RigidBody>()->CheckElasticCollisions();
    }
    
    // calculate path and set position
    for (Body* b : mBodies)
    {
        RigidBody* rb = b->GetComponent<RigidBody>();
        if (rb->GetState() == RigidBodyState::Enabled)
            b->SetPosition(rb->CalculatePath(rb->GetPathTime()));
    }

    // update trajectory
    for (Body* b : mBodies)
    {
        b->GetComponent<RigidBody>()->UpdateTrajectory();
    }    
}

void Game::GenerateOutput() {
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0);
    SDL_RenderClear(mRenderer);

    for (SpriteComponent* sprite : mSprites) {
        if (sprite->IsVisible()) {
            if (sprite->IsUI())
                sprite->DrawStaticUI(mRenderer);
            else if (sprite->IsUnscaleable())
                sprite->DrawUnscaleable(mRenderer);
            else
                sprite->Draw(mRenderer);
        }
    }

    SDL_RenderPresent(mRenderer);
}

void Game::AddActor(Actor* actor) {
    mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor) {
    std::vector<Actor*>::iterator it;
    it = std::find(mActors.begin(), mActors.end(), actor);

    if (it != mActors.end()) {
        mActors.erase(it);
    }
}

void Game::LoadData()
{
    mCamera = new Camera;
    mCamera->position = Vector2::Zero;
    mCamera->rotation = 0.0f;
    mCamera->scale = 1.0f;

    // initializing nodes
    int rows = mGridSize.second;
    int columns = mGridSize.first;
    for (int j = 0; j < rows; j++)
    {
        std::vector<Node*> row;
        mNodeGrid.push_back(row);

        for (int i = 0; i < columns; i++)
        {
            Node* n = new Node;
            n->position.y = 256.0f + ((-rows / 2) + j) * 512.0f;
            n->position.x = 256.0f + ((-columns / 2) + i) * 512.0f;
            mAllNodes.push_back(n); // put it in total node vector
            mNodeGrid[j].push_back(n); // put it in the grid
        }
    }
    // setting adjacent nodes
    for (int j = 0; j < rows; j++)
    {
        for (int i = 0; i < columns; i++)
        {
            // left right up down
            if (i > 0)
                mNodeGrid[j][i]->nearbyNodes.push_back(mNodeGrid[j][i - 1]);
            if (i < columns - 1)
                mNodeGrid[j][i]->nearbyNodes.push_back(mNodeGrid[j][i + 1]);
            if (j > 0)
                mNodeGrid[j][i]->nearbyNodes.push_back(mNodeGrid[j - 1][i]);
            if (j < rows - 1)
                mNodeGrid[j][i]->nearbyNodes.push_back(mNodeGrid[j + 1][i]);
            // diagonals
            if (i > 0 && j > 0)
                mNodeGrid[j][i]->nearbyNodes.push_back(mNodeGrid[j - 1][i - 1]);
            if (i < columns - 1 && j > 0)
                mNodeGrid[j][i]->nearbyNodes.push_back(mNodeGrid[j - 1][i + 1]);
            if (i > 0 && j < rows - 1)
                mNodeGrid[j][i]->nearbyNodes.push_back(mNodeGrid[j + 1][i - 1]);
            if (i < columns - 1 && j < rows - 1)
                mNodeGrid[j][i]->nearbyNodes.push_back(mNodeGrid[j + 1][i + 1]);
        }
    }

    mSelector = new Selector(this);
    mSelector->GetComponent<SpriteComponent>()->SetIsVisible(false);

    mInitPanel = new InitPlanetUI(this);
    mInitPanel->GetComponent<SpriteComponent>()->SetIsVisible(false);

    //InitPlanetUI* temp = new InitPlanetUI(this);
    //temp->SetScale(0.05f);
    //temp->HideSliders();
    //temp->SetPosition(mInitPanel->GetPosition());

    Vector2 origin (0.0f, 0.0f);
    Body* sun = new Body(this);
    sun->SetScale(10.0f);
    sun->GetComponent<RigidBody>()->SetMass(1000000.0f);
    sun->GetComponent<CollisionComponent>()->SetRadius(160.0f);
    sun->GetComponent<SpriteComponent>()->SetTexture(GetTexture("assets/sun.png"));
    sun->SetPosition(origin);
    mBodies.push_back(sun);
    sun->SetState(ActorState::Paused);
}

void Game::UnloadData() {
    if (!mActors.empty()) {
        mActors.erase(mActors.begin(), mActors.end());
    }

    for (auto it = mTextureCache.begin(); it != mTextureCache.end(); it++) {
        SDL_DestroyTexture(it->second);
    }

    mTextureCache.clear();
}

SDL_Texture* Game::GetTexture(const char* fileName) {
    std::unordered_map<std::string, SDL_Texture*>::const_iterator got = mTextureCache.find(fileName);
    if (got != mTextureCache.end())
        return got->second;
    else {
        SDL_Surface* tempSurface = IMG_Load(fileName);
        SDL_Texture* tempTexture = SDL_CreateTextureFromSurface(mRenderer, tempSurface);
        SDL_FreeSurface(tempSurface);
        mTextureCache.emplace(fileName, tempTexture);
        return tempTexture;
    }
}

void Game::AddSprite(SpriteComponent* sprite) {

    mSprites.emplace_back(sprite);
    std::sort(mSprites.begin(), mSprites.end(),
        [](SpriteComponent* a, SpriteComponent* b) {
            return a->GetDrawOrder() < b->GetDrawOrder();
        });
}

void Game::RemoveSprite(SpriteComponent* sprite) {
    std::vector<SpriteComponent*>::iterator it;
    it = std::find(mSprites.begin(), mSprites.end(), sprite);

    if (it != mSprites.end()) {
        mSprites.erase(it);
    }
}

void Game::DestroyMarkers()
{
    // Destroy all markers
    for (TrajectoryMarker* tMarker : mAllMarkers)
        tMarker->SetState(ActorState::Destroy);

    mAllMarkers.clear();

    // Clear all marker vectors
    for (Body* b : mBodies)
        b->GetComponent<RigidBody>()->ClearMarkers();
}

void Game::DebugStats()
{
    //system("CLS");
    std::cout << "Actor Count: " << mActors.size() << std::endl;
    std::cout << "Body Count: " << mBodies.size() << std::endl;
    std::cout << "Zoom Scale: " << mCamera->scale << std::endl;
    std::cout << "===============================" << std::endl;
}