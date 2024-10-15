#ifndef CUSTOMCLASSES_HPP
#define CUSTOMCLASSES_HPP

#include<iostream>
#include<SDL2/SDL.h>

#include <map>
#include <utility>
#include <vector>

class Vector2{
public:
    float x, y;
    Vector2();
    Vector2(float x, float y);
    Vector2 operator+(Vector2 v);
    Vector2 operator-(Vector2 v);
    Vector2 operator*(float f);
    
    float Magnitude();
    Vector2 Normalize();
    float Distance(Vector2 v);
    static float Dot(Vector2 v1, Vector2 v2);
};

/*Singleton manager for GameObjects, automatic memory management
*/
class GameObjectManager{
private:
    std::map<std::string, GameObject*> gameObjects;
    GameObjectManager();
    static GameObjectManager* Instance();

public:
    ~GameObjectManager();
    static GameObjectManager* GetInstance();

    void AddGameObject(std::string name, GameObject* gameObject);
    void RemoveGameObject(std::string name);
    GameObject* GetGameObject(std::string name);

    void Update();
    void Draw();
};

class Transform{
public:
    Vector2 position, rotation, scale;
    Transform();
    Transform(Vector2 position, Vector2 rotation, Vector2 scale);
};

class Component {
public:
    GameObject* gameObject;
    virtual ~Component() {}
    virtual void Update() = 0;
    virtual void Draw() = 0;
};

class SpriteRenderer : public Component {
public:
    SDL_Texture* texture;
    SDL_Rect rect;
    SpriteRenderer();
    ~SpriteRenderer();
    void Update();
    void Draw();
};

class GameObject {
private:
    std::vector<Component*> components;

public:
    Transform* transform;
    GameObject();
    ~GameObject();
    void Update();
    void Draw();
    
    template <typename T>
    T* AddComponent() {
        T* component = new T();
        component->gameObject = this;
        components.push_back(component);
        return component;
    }

    template <typename T>
    T* GetComponent() {
        for (Component* component : components) {
            T* t = dynamic_cast<T*>(component);
            if (t != nullptr) {
                return t;
            }
        }
        return nullptr;
    }

    static GameObject* Instantiate(std::string name, const GameObject& origin, std::pair<float, float> position, std::pair<float, float> rotation, std::pair<float, float> scale);
    static GameObject* Instantiate(std::string name, const GameObject& origin, Vector2 position, Vector2 rotation, Vector2 scale);
    static void Destroy(std::string name);
};
#endif