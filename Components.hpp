#include "CustomClasses.hpp"
#include "Physic2D.hpp"
#include "Game.hpp"

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
