#include "CustomClasses.hpp"
#include <iostream>
#include <cmath>

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

GameObjectManager* GameObjectManager::Instance() {
    static GameObjectManager instance;
    return &instance;
}

GameObjectManager::~GameObjectManager() {
    for (auto& pair : gameObjects) {
        delete pair.second;
    }
    gameObjects.clear();
}

GameObjectManager* GameObjectManager::GetInstance() {
    return Instance();
}

void GameObjectManager::AddGameObject(std::string name, GameObject* gameObject) {
    gameObjects[name] = gameObject;
}

void GameObjectManager::RemoveGameObject(std::string name) {
    auto it = gameObjects.find(name);
    if (it != gameObjects.end()) {
        delete it->second;
        gameObjects.erase(it);
    }
}

GameObject* GameObjectManager::GetGameObject(std::string name) {
    auto it = gameObjects.find(name);
    if (it != gameObjects.end()) {
        return it->second;
    }
    return nullptr;
}

void GameObjectManager::Update() {
    for (auto& pair : gameObjects) {
        pair.second->Update();
    }
}

void GameObjectManager::Draw() {
    for (auto& pair : gameObjects) {
        pair.second->Draw();
    }
}

#pragma endregion

#pragma region GameObject
// GameObject class implementation
class Transform {
public:
    Vector2 position, rotation, scale;

    Transform() : position(Vector2()), rotation(Vector2()), scale(Vector2(1, 1)) {}

    Transform(Vector2 position, Vector2 rotation, Vector2 scale)
        : position(position), rotation(rotation), scale(scale) {}
};

GameObject::GameObject() {}

GameObject::~GameObject() {
    delete transform;
}

void GameObject::Update() {
    // Update logic for the game object
}

void GameObject::Draw() {
    // // Assuming SDL_Renderer* renderer is available in the scope
    // SDL_Rect rect;
    // rect.x = static_cast<int>(transform->position.x);
    // rect.y = static_cast<int>(transform->position.y);
    // rect.w = static_cast<int>(transform->scale.x);
    // rect.h = static_cast<int>(transform->scale.y);

    // // Set the drawing color (e.g., white)
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // // Draw the rectangle
    // SDL_RenderFillRect(renderer, &rect);
}

GameObject* GameObject::Instantiate(std::string name, const GameObject& origin, std::pair<float, float> position, std::pair<float, float> rotation, std::pair<float, float> scale) {
    GameObject* newObject = new GameObject(origin);
    
    // Set position, rotation and scale
    newObject->transform = new Transform(Vector2(position.first, position.second), Vector2(rotation.first, rotation.second), Vector2(scale.first, scale.second));

    // Add to manager
    GameObjectManager::GetInstance()->AddGameObject(name, newObject);
    return newObject;
}

GameObject* GameObject::Instantiate(std::string name, const GameObject& origin,  Vector2 position, Vector2 rotation, Vector2 scale) {
    GameObject* newObject = new GameObject(origin);
    
    // Set position, rotation and scale
    newObject->transform = new Transform(position, rotation, scale);

    // Add to manager
    GameObjectManager::GetInstance()->AddGameObject(name, newObject);
    return newObject;
}

void GameObject::Destroy(std::string name) {
    GameObjectManager::GetInstance()->RemoveGameObject(name);
}
#pragma endregion