#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "CustomClasses.hpp"
#include "Physic2D.hpp"
#include "Game.hpp"

//Player Control
class MovementController : public Component {

private:
    Rigidbody2D *rigidbody;
    SDL_Keycode upKey, downKey, leftKey, rightKey;

    float upSpeed = 0, downSpeed = 0, leftSpeed = 0, rightSpeed = 0;

    bool enabled = true;

public:
    float speed = 0;
    
    MovementController(GameObject* parent, float speed, bool useWASD): Component(parent){
        this->speed = speed;
        this->rigidbody = this->gameObject->GetComponent<Rigidbody2D>();

        if (useWASD){
            upKey = SDLK_w;
            downKey = SDLK_s;
            leftKey = SDLK_a;
            rightKey = SDLK_d;
        }
        else{
            upKey = SDLK_UP;
            downKey = SDLK_DOWN;
            leftKey = SDLK_LEFT;
            rightKey = SDLK_RIGHT;
        }
    }

    void Update(){
        if (!enabled) return;
        if (rigidbody == nullptr) return;

        float actualSpeed = speed * 1/FPS;

        if (Game::event.type == SDL_KEYDOWN || Game::event.type == SDL_KEYUP){
            if (Game::event.key.keysym.sym == upKey){
                upSpeed = Game::event.type == SDL_KEYDOWN ? -1 : 0;
            }
            if (Game::event.key.keysym.sym == downKey){
                downSpeed = Game::event.type == SDL_KEYDOWN ? 1 : 0;
            }
            if (Game::event.key.keysym.sym == leftKey){
                leftSpeed = Game::event.type == SDL_KEYDOWN ? -1 : 0;
            }
            if (Game::event.key.keysym.sym == rightKey){
                rightSpeed = Game::event.type == SDL_KEYDOWN ? 1 : 0;
            }
        }

        rigidbody->AddForce(Vector2(leftSpeed + rightSpeed, upSpeed + downSpeed). Normalize() * actualSpeed);
    }

    void Enable(){
        enabled = true;
    }
    void Disable(){
        enabled = false;
    }
    bool GetEnabled(){
        return enabled;
    }

    void Draw(){}

    Component* Clone(GameObject* parent){
        MovementController* newMovementController = new MovementController(parent, speed, upKey == SDLK_w);
        return newMovementController;
    }

};

class MovementControllerSwitcher : public Component {
private:
    std::map<int, MovementController*> movementControllers;
    int currentKey = -1;
public:

    MovementControllerSwitcher(GameObject* parent): Component(parent){}

    void Update(){
        if (Game::event.type == SDL_KEYDOWN){
            SDL_Keycode key = Game::event.key.keysym.sym;
            if (movementControllers.find(key) != movementControllers.end()){
                for (auto &movementController : movementControllers){
                    movementController.second->Disable();
                }
                movementControllers[key]->Enable();
            }
        }
    }

    void Draw(){}

    void AddMovementController(int keyBind, MovementController* movementController){
        movementControllers[keyBind] = movementController;

        //Enable only the first movement controller when adding
        if (movementControllers.size() > 1){
            movementController->Disable();
        }
    }

    void RemoveMovementController(int keyBind){
        movementControllers[keyBind] = nullptr;

        //Enable the first controller when removing
        if (movementControllers.find(keyBind) != movementControllers.end()){
            movementControllers.begin()->second->Enable();
        }
    }

    Component* Clone(GameObject* parent){
        MovementControllerSwitcher* newMovementControllerSwitcher = new MovementControllerSwitcher(parent);
        for (auto &movementController : movementControllers){
            newMovementControllerSwitcher->AddMovementController(movementController.first, movementController.second);
        }
        return newMovementControllerSwitcher;
    }
    
};

//AI Control

//Auto take over the control of the object if movement controller is not enabled
class AIController : public Component {
protected:
    Rigidbody2D *rigidbody;
    MovementController *movementController;

    GameObject *target;

    float speed = 0;

    float alertZoneXStart = 0, alertZoneXEnd = 0;
    float dangerZoneXStart = 0, dangerZoneXEnd = 0;

    bool isTeam1;
public:

    AIController(GameObject* parent, GameObject* target, float speed, bool isTeam1): Component(parent){
        this->rigidbody = this->gameObject->GetComponent<Rigidbody2D>();
        this->movementController = this->gameObject->GetComponent<MovementController>();
        this->target = target;
        this->speed = speed;

        this->isTeam1 = isTeam1;
    }

    virtual void Update() = 0;

    void Draw(){}

    virtual Component* Clone(GameObject* parent) = 0;
};

class AIGoalKeeper : public AIController {
private:
    float alertZoneYStart = 0, alertZoneYEnd = 0;
public:
    AIGoalKeeper(GameObject* parent, GameObject* target, float speed, bool isTeam1): AIController(parent, target, speed, isTeam1){
        if (isTeam1) {
            dangerZoneXStart = 0;
            dangerZoneXEnd = 25.0f / 100.0f * WIDTH;

            alertZoneXStart = 25.0f / 100.0f * WIDTH;
            alertZoneXEnd = 50.0f / 100.0f * WIDTH;
        } else {
            dangerZoneXStart = 75.0f / 100.0f * WIDTH;
            dangerZoneXEnd = 100.0f / 100.0f * WIDTH;

            alertZoneXStart = 50.0f / 100.0f * WIDTH;
            alertZoneXEnd = 75.0f / 100.0f * WIDTH;
        }

        alertZoneYStart = 40.0f / 100.0f * HEIGHT;
        alertZoneYEnd = 60.0f / 100.0f * HEIGHT;
    }

    void Update(){
        if (rigidbody == nullptr) return;
        if (movementController != nullptr && movementController->GetEnabled()) return;

        float actualSpeed = speed * 1/FPS;

        Vector2 targetPosition = target->transform.position;
        Vector2 currentPosition = gameObject->transform.position;
        
        //Target is in the alert zone
        if (targetPosition.x >= alertZoneXStart && targetPosition.x <= alertZoneXEnd) {
            if (targetPosition.y < currentPosition.y) {
                rigidbody->AddForce(Vector2(0, -1).Normalize() * actualSpeed);
            } else if (targetPosition.y > currentPosition.y) {
                rigidbody->AddForce(Vector2(0, 1).Normalize() * actualSpeed);
            }
        }

        //Target is in the danger zone
        else
        if (targetPosition.x >= dangerZoneXStart && targetPosition.x <= dangerZoneXEnd &&
            targetPosition.y >= alertZoneYStart && targetPosition.y <= alertZoneYEnd) {
            Vector2 direction = (targetPosition - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }

        //Target is neither, restore original position
        else{
            Vector2 dangerZoneCenter((dangerZoneXStart + dangerZoneXEnd) / 2, (alertZoneYStart + alertZoneYEnd) / 2);
            Vector2 direction = (dangerZoneCenter - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }
    }

    Component* Clone(GameObject* parent){
        AIGoalKeeper* newAIGoalKeeper = new AIGoalKeeper(parent, target, speed, isTeam1);
        return newAIGoalKeeper;
    }
};

class AIDefender : public AIController {
public:
    AIDefender(GameObject* parent, GameObject* target, float speed, bool isTeam1): AIController(parent, target, speed, isTeam1){
        if (isTeam1) {
            dangerZoneXStart = 0.0f / 100.0f * WIDTH;
            dangerZoneXEnd = 50.0f / 100.0f * WIDTH;

            alertZoneXStart = 50.0f / 100.0f * WIDTH;
            alertZoneXEnd = 100.0f / 100.0f * WIDTH;
        } else {
            dangerZoneXStart = 50.0f / 100.0f * WIDTH;
            dangerZoneXEnd = 100.0f / 100.0f * WIDTH;

            alertZoneXStart = 0.0f;
            alertZoneXEnd = 50.0f / 100.0f * WIDTH;
        }
    }

    void Update(){
        if (rigidbody == nullptr) return;
        if (movementController != nullptr && movementController->GetEnabled()) return;

        float actualSpeed = speed * 1/FPS;

        Vector2 targetPosition = target->transform.position;
        Vector2 currentPosition = gameObject->transform.position;
        
        //Target is in the alert zone
        if (targetPosition.x >= alertZoneXStart && targetPosition.x <= alertZoneXEnd) {
            Vector2 direction = (targetPosition - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }

        //Target is in the danger zone
        else if (targetPosition.x >= dangerZoneXStart && targetPosition.x <= dangerZoneXEnd) {
            Vector2 direction = (targetPosition - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }

        //Target is neither, restore original position
        else{
            Vector2 dangerZoneCenter((dangerZoneXStart + dangerZoneXEnd) / 2, currentPosition.y);
            Vector2 direction = (dangerZoneCenter - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }
    }

    Component* Clone(GameObject* parent){
        AIDefender* newAIDefender = new AIDefender(parent, target, speed, isTeam1);
        return newAIDefender;
    }
};

class AIAttacker : public AIController {
public:
    AIAttacker(GameObject* parent, GameObject* target, float speed, bool isTeam1): AIController(parent, target, speed, isTeam1){
        if (isTeam1) {
            dangerZoneXStart = 50.0f / 100.0f * WIDTH;
            dangerZoneXEnd = 100.0f / 100.0f * WIDTH;

            alertZoneXStart = 100.0f / 100.0f * WIDTH;
            alertZoneXEnd = 100.0f / 100.0f * WIDTH;
        } else {
            dangerZoneXStart = 0.0f;
            dangerZoneXEnd = 50.0f / 100.0f * WIDTH;

            alertZoneXStart = 0.0f / 100.0f * WIDTH;
            alertZoneXEnd = 0.0f / 100.0f * WIDTH;
        }
    }

    void Update(){
        if (rigidbody == nullptr) return;
        if (movementController != nullptr && movementController->GetEnabled()) return;

        float actualSpeed = speed * 1/FPS;

        Vector2 targetPosition = target->transform.position;
        Vector2 currentPosition = gameObject->transform.position;
        
        //Target is in the alert zone
        if (targetPosition.x >= alertZoneXStart && targetPosition.x <= alertZoneXEnd) {
            Vector2 direction = (targetPosition - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }

        //Target is in the danger zone
        else if (targetPosition.x >= dangerZoneXStart && targetPosition.x <= dangerZoneXEnd) {
            Vector2 direction = (targetPosition - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }

        //Target is neither, restore original position
        else{
            Vector2 dangerZoneCenter((dangerZoneXStart + dangerZoneXEnd) / 2, currentPosition.y);
            Vector2 direction = (dangerZoneCenter - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }
    }

    Component* Clone(GameObject* parent){
        AIAttacker* newAIAttacker = new AIAttacker(parent, target, speed, isTeam1);
        return newAIAttacker;
    }
};

#endif
