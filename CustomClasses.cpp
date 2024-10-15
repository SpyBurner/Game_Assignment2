#include "CustomClasses.hpp"
#include <cmath>
#include <iostream>

#pragma region Vector2
// Vector2 class implementation
Vector2::Vector2() : x(0), y(0) {}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

Vector2 Vector2::operator+(Vector2 v) {
    return Vector2(x + v.x, y + v.y);
}

Vector2 Vector2::operator-(Vector2 v) {
    return Vector2(x - v.x, y - v.y);
}

Vector2 Vector2::operator*(float f) {
    return Vector2(x * f, y * f);
}

float Vector2::Magnitude() {
    return std::sqrt(x * x + y * y);
}

Vector2 Vector2::Normalize() {
    float magnitude = Magnitude();
    return Vector2(x / magnitude, y / magnitude);
}

float Vector2::Distance(Vector2 v) {
    return std::sqrt((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y));
}

float Vector2::Dot(Vector2 v1, Vector2 v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

#pragma endregion

#pragma region GameObjectManager
// GameObjectManager class implementation
GameObjectManager::GameObjectManager() {}

GameObjectManager::~GameObjectManager() {
    for (auto &pair : gameObjects) {
        delete pair.second;
    }
    gameObjects.clear();
}

GameObjectManager* GameObjectManager::GetInstance() {
    if (instance == nullptr) {
        instance = new GameObjectManager();
    }
    return instance;
}

void GameObjectManager::AddGameObject(std::vector<std::pair<std::string, GameObject *>> gameObjects) {
    for (auto &pair : gameObjects) {
        this->gameObjects[pair.first] = pair.second;
    }
}

void GameObjectManager::AddGameObject(std::string name, GameObject *gameObject) {
    gameObjects[name] = gameObject;
}

void GameObjectManager::RemoveGameObject(std::string name) {
    auto it = gameObjects.find(name);
    if (it != gameObjects.end()) {
        delete it->second;
        gameObjects.erase(it);
    }
}

GameObject *GameObjectManager::GetGameObject(std::string name) {
    auto it = gameObjects.find(name);
    if (it != gameObjects.end()) {
        return it->second;
    }
    return nullptr;
}

void GameObjectManager::Update() {
    for (auto &pair : gameObjects) {
        pair.second->Update();
    }
}

void GameObjectManager::Draw() {
    for (auto &pair : gameObjects) {
        pair.second->Draw();
    }
}

#pragma endregion

#pragma region GameObject
// GameObject class implementation
Transform::Transform() : position(Vector2(0, 0)), rotation(Vector2(0, 0)), scale(Vector2(1, 1)) {}
Transform::Transform(Vector2 position, Vector2 rotation, Vector2 scale) {
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
}

GameObject::GameObject() {
    transform = new Transform();
}

GameObject::~GameObject() {
    delete transform;
}

void GameObject::Update() {
    for (auto &component : components) {
        component->Update();
    }
}

void GameObject::Draw() {
    for (auto &component : components) {
        component->Draw();
    }
}

GameObject *GameObject::Instantiate(std::string name, const GameObject &origin, std::pair<float, float> position, std::pair<float, float> rotation, std::pair<float, float> scale) {
    return Instantiate(name, origin, Vector2(position.first, position.second), Vector2(rotation.first, rotation.second), Vector2(scale.first, scale.second));
}

GameObject *GameObject::Instantiate(std::string name, const GameObject &origin, Vector2 position, Vector2 rotation, Vector2 scale) {
    GameObject *newObject = new GameObject(origin);

    // Set position, rotation and scale
    newObject->transform = new Transform(position, rotation, scale);

    // Deep copy components
    for (auto &component : origin.components) {
        Component *newComponent = component->Clone(newObject);
        newObject->components.push_back(newComponent);
    }

    // Add to manager
    GameObjectManager::GetInstance()->AddGameObject(name, newObject);
    return newObject;
}

void GameObject::Destroy(std::string name) {
    GameObjectManager::GetInstance()->RemoveGameObject(name);
}
#pragma endregion

#pragma region Component
// Component classes implementation
Component::Component(GameObject *parent) : gameObject(parent) {}

// SpriteRenderer class implementation
SpriteRenderer::SpriteRenderer(GameObject *gameObject, SDL_Renderer *renderer, Vector2 spriteSize, SDL_Texture *defaultSpriteSheet = nullptr) : Component(gameObject) {
    this->renderer = renderer;
    this->spriteSheet = spriteSheet;
}

SpriteRenderer::~SpriteRenderer() {}

void SpriteRenderer::Update() {}

void SpriteRenderer::Draw() {
    SDL_Rect destRect;
    // Moving the rect to center the sprite
    destRect.x = gameObject->transform->position.x - spriteRect.w / 2;
    destRect.y = gameObject->transform->position.y - spriteRect.h / 2;
    destRect.w = spriteRect.w;
    destRect.h = spriteRect.h;

    SDL_RenderCopy(renderer, spriteSheet, &spriteRect, &destRect);
}

SpriteRenderer* SpriteRenderer::Clone(GameObject *parent) {
    SpriteRenderer *newRenderer = new SpriteRenderer(parent, renderer, Vector2(spriteRect.w, spriteRect.h), spriteSheet);
    newRenderer->spriteRect = spriteRect;
    return newRenderer;
}

// AnimationClip class implementation
AnimationClip::AnimationClip(std::string name, std::string path, Vector2 spriteSize,
                             float length, bool loop, float speedScale) {
    this->name = name;
    this->spriteSize = spriteSize;
    this->length = length;
    this->loop = loop;
    this->speedScale = speedScale;

    spriteSheet = SpriteRenderer::LoadSpriteSheet(path);
}

AnimationClip::~AnimationClip() {
    SDL_DestroyTexture(spriteSheet);
}

void AnimationClip::AdvanceFrame() {
    if (!isPlaying)
        return;

    float currentTime = SDL_GetTicks();
    if (currentTime - lastFrameTime <= animCooldown)
        return;

    lastFrameTime = currentTime;
    currentSprite++;

    if (loop) {
        currentSprite = 0;
    } else {
        isPlaying = false;
        if (onComplete != nullptr) {
            SDL_PushEvent(onComplete);
        }
    }
}

// Animator class implementation

#pragma endregion