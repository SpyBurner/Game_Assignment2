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

GameObjectManager *GameObjectManager::GetInstance() {
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

void GameObjectManager::Clear() {
    for (auto &pair : gameObjects) {
        delete pair.second;
    }
    gameObjects.clear();
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
}

GameObject::GameObject(std::string name) {
    this->name = name;
}

GameObject::~GameObject() {
    for (auto &component : components) {
        delete component;
    }
    components.clear();
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

    // Deep copy transform
    newObject->transform = origin.transform;

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

#pragma region SpriteRenderer
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
    destRect.x = gameObject->transform.position.x - spriteRect.w / 2;
    destRect.y = gameObject->transform.position.y - spriteRect.h / 2;
    destRect.w = spriteRect.w;
    destRect.h = spriteRect.h;

    SDL_RenderCopy(renderer, spriteSheet, &spriteRect, &destRect);
}

Component *SpriteRenderer::Clone(GameObject *parent) {
    SpriteRenderer *newRenderer = new SpriteRenderer(parent, renderer, Vector2(spriteRect.w, spriteRect.h), spriteSheet);
    newRenderer->spriteRect = spriteRect;

    return newRenderer;
}

#pragma endregion

#pragma region Animator
// AnimationClip class implementation
AnimationClip::AnimationClip(std::string name, std::string path, Vector2 spriteSize,
                             float length, bool loop, float speedScale) {
    // Stats setup
    this->name = name;
    this->spriteSize = spriteSize;
    this->length = length;
    this->loop = loop;
    this->speedScale = speedScale;

    // Sprite setup
    spriteSheet = SpriteRenderer::LoadSpriteSheet(path);

    currentSprite = startSprite;

    currentSpriteRect.x = currentSprite * spriteSize.x;
    currentSpriteRect.y = currentSprite * spriteSize.y;

    currentSpriteRect.w = spriteSize.x;
    currentSpriteRect.h = spriteSize.y;

    onComplete = new Event();
}

AnimationClip::~AnimationClip() {
    SDL_DestroyTexture(spriteSheet);
    delete onComplete;
}

void AnimationClip::AdvanceFrame() {
    if (!isPlaying)
        return;

    float currentTime = SDL_GetTicks();
    if (currentTime - lastFrameTime <= animCooldown)
        return;

    lastFrameTime = currentTime;
    currentSprite++;

    if (currentSprite >= endSprite) {
        if (loop) {
            currentSprite = startSprite;
        } else {
            currentSprite = endSprite;
            isPlaying = false;
            if (onComplete != nullptr) {
                onComplete->raise();
            }
        }
    }
}

void AnimationClip::Ready() {
    currentSprite = startSprite;
    isPlaying = true;
    startTime = SDL_GetTicks();
    lastFrameTime = SDL_GetTicks() - animCooldown * speedScale;
}

// Animator class implementation
Animator::Animator(GameObject *gameObject, std::vector<AnimationClip> clips) : Component(gameObject) {
    for (auto &clip : clips) {
        this->clips[clip.name] = clip;
    }
    currentClip = &(clips[0]);
}

Animator::~Animator() {
    clips.clear();
}

void Animator::Update() {
    if (currentClip)
        currentClip->AdvanceFrame();
    std::pair<SDL_Texture *, Vector2> sprite = currentClip->GetCurrentSprite();

    SpriteRenderer *renderer = gameObject->GetComponent<SpriteRenderer>();
    if (renderer) {
        renderer->spriteSheet = sprite.first;
        renderer->spriteRect = currentClip->currentSpriteRect;
    }
}

void Animator::Draw() {}

void Animator::Play(std::string name) {
    AnimationClip *clip = GetClip(name);
    if (clip) {
        currentClip = clip;
        currentClip->Ready();
    }
}

void Animator::Stop() {
    currentClip->isPlaying = false;
}

AnimationClip *Animator::GetClip(std::string name) {
    auto it = clips.find(name);
    if (it != clips.end()) {
        return &(it->second);
    }
    return nullptr;
}

std::vector<AnimationClip> Animator::GetAllClips() {
    std::vector<AnimationClip> clipList;
    for (auto &pair : clips) {
        clipList.push_back(pair.second);
    }
    return clipList;
}

Component *Animator::Clone(GameObject *parent) {
    Animator *newAnimator = new Animator(parent, GetAllClips());
    return newAnimator;
}

#pragma endregion

#pragma endregion

#pragma region SceneManager
// Scene class implementation
Scene::Scene(std::string name) {
    this->name = name;
}

Scene::~Scene() {
    for (auto &pair : gameObjects) {
        delete pair.second;
    }
    gameObjects.clear();
}

void Scene::AssignLogic(std::function<void()> logic) {
    this->logic = logic;
}

void Scene::RunLogic() {
    if (logic) {
        logic();
    }
}

void Scene::AddGameObject(std::string name, GameObject *gameObject) {
    gameObjects[name] = gameObject;
}

void Scene::RemoveGameObject(std::string name) {
    auto it = gameObjects.find(name);
    if (it != gameObjects.end()) {
        delete it->second;
        gameObjects.erase(it);
    }
}

void Scene::Load() {
    // Clear all objects
    GameObjectManager::GetInstance()->Clear();

    for (auto &pair : gameObjects) {
        GameObjectManager::GetInstance()->AddGameObject(pair.first, pair.second);
    }
}

std::string Scene::GetName() {
    return name;
}

// SceneManager class implementation
SceneManager::SceneManager() {}
SceneManager::~SceneManager() {
    for (auto &pair : scenes) {
        delete pair.second;
    }
    scenes.clear();
}

SceneManager *SceneManager::GetInstance() {
    if (instance == nullptr) {
        instance = new SceneManager();
    }
    return instance;
}

void SceneManager::RunLogic() {
    if (currentScene) {
        currentScene->RunLogic();
    }
}

void SceneManager::AddScene(Scene *scene) {
    scenes[scene->GetName()] = scene;
}

void SceneManager::LoadScene(std::string name) {
    auto it = scenes.find(name);
    if (it != scenes.end()) {
        if (currentScene) {
            delete currentScene;
        }
        currentScene = it->second;
        currentScene->Load();
    }
}

Scene *SceneManager::GetCurrentScene() {
    return currentScene;
}

void SceneManager::Update() {
    GameObjectManager::GetInstance()->Update();
}

void SceneManager::Draw() {
    GameObjectManager::GetInstance()->Draw();
}

#pragma endregion
