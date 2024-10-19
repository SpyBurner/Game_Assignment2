#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "CustomClasses.hpp"
#include "Game.hpp"
#include "Physic2D.hpp"
#include "cmath"

// Player cosmetics
class RotateTowardVelocity : public Component {
private:
    Vector2 originalForward = Vector2(0, -1);
    Rigidbody2D *rigidbody = nullptr;
    
    Vector2 lastVelocity = Vector2(0, -1);

    void Update(){
        if (rigidbody == nullptr){
            rigidbody = gameObject->GetComponent<Rigidbody2D>();
        }
        if (rigidbody == nullptr) return;

        if (rigidbody->velocity.Magnitude() > 0.1f) lastVelocity = rigidbody->velocity;
        gameObject->transform.rotation = Vector2::SignedAngle(originalForward, lastVelocity);
    }

    void Draw() {}

    Component *Clone(GameObject *parent) {
        RotateTowardVelocity *newRotateTowardVelocity = new RotateTowardVelocity(parent, originalForward);
        return newRotateTowardVelocity;
    }
public:
    RotateTowardVelocity(GameObject *parent, Vector2 originalForward) : Component(parent) {
        this->originalForward = originalForward;
        rigidbody = gameObject->GetComponent<Rigidbody2D>();
    }
};

// Player Control
class MovementController : public Component {

private:
    Rigidbody2D *rigidbody;
    SDL_Keycode upKey, downKey, leftKey, rightKey;

    float upSpeed = 0, downSpeed = 0, leftSpeed = 0, rightSpeed = 0;

    bool enabled = true;

public:
    float speed = 0;

    MovementController(GameObject *parent, float speed, bool useWASD) : Component(parent) {
        this->speed = speed;
        this->rigidbody = this->gameObject->GetComponent<Rigidbody2D>();

        if (useWASD) {
            upKey = SDLK_w;
            downKey = SDLK_s;
            leftKey = SDLK_a;
            rightKey = SDLK_d;
        } else {
            upKey = SDLK_UP;
            downKey = SDLK_DOWN;
            leftKey = SDLK_LEFT;
            rightKey = SDLK_RIGHT;
        }
    }

    void Update() {
        if (!enabled)
            return;
        if (rigidbody == nullptr)
            return;

        float actualSpeed = speed * 1 / FPS;

        if (Game::event.type == SDL_KEYDOWN || Game::event.type == SDL_KEYUP) {
            if (Game::event.key.keysym.sym == upKey) {
                upSpeed = Game::event.type == SDL_KEYDOWN ? -1 : 0;
            }
            if (Game::event.key.keysym.sym == downKey) {
                downSpeed = Game::event.type == SDL_KEYDOWN ? 1 : 0;
            }
            if (Game::event.key.keysym.sym == leftKey) {
                leftSpeed = Game::event.type == SDL_KEYDOWN ? -1 : 0;
            }
            if (Game::event.key.keysym.sym == rightKey) {
                rightSpeed = Game::event.type == SDL_KEYDOWN ? 1 : 0;
            }
        }

        rigidbody->AddForce(Vector2(leftSpeed + rightSpeed, upSpeed + downSpeed).Normalize() * actualSpeed);
    }

    void Enable() {
        ClearSpeed();
        enabled = true;
    }
    void Disable() {
        ClearSpeed();
        enabled = false;
    }
    bool GetEnabled() {
        return enabled;
    }

    void ClearSpeed(){
        upSpeed = 0;
        downSpeed = 0;
        leftSpeed = 0;
        rightSpeed = 0;
    }

    void Draw() {}

    Component *Clone(GameObject *parent) {
        MovementController *newMovementController = new MovementController(parent, speed, upKey == SDLK_w);
        return newMovementController;
    }
};

class TeamControl : public Component {
private:
    SDL_Texture *indicator = nullptr;
    float indicatorRadius = 0;

    std::map<int, MovementController *> movementControllers;
    int currentKey = -1;

public:
    TeamControl(GameObject *parent, SDL_Texture *indicator, float indicatorRadius) : Component(parent) {
        this->indicator = indicator;
        this->indicatorRadius = indicatorRadius;
    }

    void Update() {
        if (Game::event.type == SDL_KEYDOWN) {
            SDL_Keycode key = Game::event.key.keysym.sym;
            if (movementControllers.find(key) != movementControllers.end()) {
                movementControllers[currentKey]->Disable();

                movementControllers[key]->Enable();
                currentKey = key;
            }
        }
    }

    void Draw() {
        if (currentKey != -1) {
            MovementController *currentController = movementControllers[currentKey];
            GameObject *currentObject = currentController->gameObject;
            SpriteRenderer *spriteRenderer = currentObject->GetComponent<SpriteRenderer>();

            if (spriteRenderer != nullptr) {
                SDL_Rect rect = {
                    static_cast<int>(currentObject->transform.position.x - indicatorRadius / 2),
                    static_cast<int>(currentObject->transform.position.y - indicatorRadius / 2),
                    static_cast<int>(indicatorRadius),
                    static_cast<int>(indicatorRadius)
                };
                SDL_RenderCopy(RENDERER, indicator, nullptr, &rect);
            }
        }
    }

    void AddMovementController(int keyBind, MovementController *movementController) {
        movementControllers[keyBind] = movementController;
        
        // Enable only the first movement controller when adding
        if (movementControllers.size() > 1) {
            movementController->Disable();
        }
        else{
            // set the current key to the first key binded
            currentKey = keyBind;
        }
    }

    void RemoveMovementController(int keyBind) {
        movementControllers[keyBind] = nullptr;

        // Enable the first controller when removing
        if (movementControllers.find(keyBind) != movementControllers.end()) {
            movementControllers.begin()->second->Enable();
        }
    }

    Component *Clone(GameObject *parent) {
        TeamControl *newMovementControllerSwitcher = new TeamControl(parent, indicator, indicatorRadius);
        for (auto &movementController : movementControllers) {
            newMovementControllerSwitcher->AddMovementController(movementController.first, movementController.second);
        }
        return newMovementControllerSwitcher;
    }
};

#endif
