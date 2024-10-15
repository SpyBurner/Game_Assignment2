#ifndef CUSTOMCLASSES_HPP
#define CUSTOMCLASSES_HPP

#include <SDL2/SDL.h>
#include <iostream>

#include <map>
#include <utility>
#include <vector>

class Vector2 {
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
class GameObjectManager {
private:
    std::map<std::string, GameObject *> gameObjects;
    GameObjectManager();
    static GameObjectManager *instance;

public:
    ~GameObjectManager();

    static GameObjectManager *GetInstance();
    void AddGameObject(std::vector<std::pair<std::string, GameObject *>> gameObjects);
    void AddGameObject(std::string name, GameObject *gameObject);
    void RemoveGameObject(std::string name);
    GameObject *GetGameObject(std::string name);

    void Update();
    void Draw();
};

class Component {
public:
    GameObject *gameObject = nullptr;
    Component(GameObject *parent);
    virtual ~Component();
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual Component* Clone(GameObject *parent) = 0;
};

class SpriteRenderer : public Component {
private:
    SDL_Renderer *renderer = nullptr;
public:
    //Controlled by the animator, DO NOT DESTROY
    SDL_Texture *spriteSheet = nullptr;
    SDL_Rect spriteRect;

    bool isFlipped;

    SpriteRenderer(GameObject *gameObject, SDL_Renderer *renderer, Vector2 spriteSize, SDL_Texture *texture = nullptr);
    ~SpriteRenderer();
    void Update();
    void Draw();
    SpriteRenderer* Clone(GameObject *parent);

    static SDL_Texture *LoadSpriteSheet(std::string path);
};

class AnimationClip {
public:
    std::string name;
    bool loop = false, isPlaying = false;
    float length = 0;

    Vector2 spriteSize = Vector2(1, 1);

    SDL_Texture *spriteSheet;

    int currentSprite = 0, startSprite = 0, endSprite = 0;

    float speedScale = 0, animCooldown = 0, lastFrameTime = 0, startTime = 0;

    SDL_Event *onComplete = nullptr;

    AnimationClip(std::string name, std::string path, Vector2 spriteSize, float length, bool loop, float speedScale);
    ~AnimationClip();

    void AdvanceFrame();
    void Ready();
    std::pair<SDL_Texture*, Vector2> GetCurrentSprite();
};

class Animator : public Component {
private:
    std::vector<AnimationClip *> clips;
    AnimationClip *currentClip = nullptr;

public:
    Animator(GameObject *gameObject, std::vector<AnimationClip *> clips);
    ~Animator();

    void Update();
    void Draw();

    void Play(std::string name);
    void Stop();

    AnimationClip *GetClip(std::string name);
    std::vector<AnimationClip *> *GetAllClips();
};

class Transform {
public:
    Vector2 position, rotation, scale;
    Transform();
    Transform(Vector2 position, Vector2 rotation, Vector2 scale);
};

class GameObject {
private:
    std::string name;
    std::vector<Component *> components;

public:
    Transform *transform;
    //Prefabs do not need a name
    GameObject();
    ~GameObject();
    void Update();
    void Draw();

    template <typename T>
    T *AddComponent() {
        T *component = new T();
        component->gameObject = this;
        components.push_back(component);
        return component;
    }

    template <typename T>
    T *GetComponent() {
        for (Component *component : components) {
            T *t = dynamic_cast<T *>(component);
            if (t != nullptr) {
                return t;
            }
        }
        return nullptr;
    }

    static GameObject *Instantiate(std::string name, const GameObject &origin, std::pair<float, float> position, std::pair<float, float> rotation, std::pair<float, float> scale);
    static GameObject *Instantiate(std::string name, const GameObject &origin, Vector2 position, Vector2 rotation, Vector2 scale);
    static void Destroy(std::string name);
};
#endif