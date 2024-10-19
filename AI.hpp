#ifndef AI_HPP
#define AI_HPP

#include "Physic2D.hpp"
#include "CustomClasses.hpp"
#include "Global.hpp"
#include "Components.hpp"
#include "Helper.hpp"

class BallStateMachine : public Component {
private:
    float maxSpeed = 0;
    float kickedStateMinSpeed = 0;

    GameObject *lastKickedBy = nullptr;

    GameObject *lastBindedBy = nullptr;
    Vector2 lastBindedVelocity = Vector2(0, 0);

    float bindCooldown = 0;
    float lastBindTime = 0;
public:

    enum State {
        FREE,
        BINDED,
        KICKED
    };

    BallStateMachine(GameObject *parent, float kickedStateMinSpeed, float bindCoolDown) : Component(parent) {
        currentState = FREE;
        this->bindCooldown = bindCoolDown;

        this->kickedStateMinSpeed = kickedStateMinSpeed;
        this->rigidbody = this->gameObject->GetComponent<Rigidbody2D>();
    }

    void OnCollisionEnter(Collider2D *other) {
        if (currentState == FREE) {
            if (other->gameObject->tag == "1" || other->gameObject->tag == "2") 
                Bind(other->gameObject);
        } else if (currentState == BINDED) {
            if (other->gameObject->tag != gameObject->tag) {
                Bind(other->gameObject);
            }
        } else if (currentState == KICKED) {
            if (other->gameObject->tag == "1" || other->gameObject->tag == "2") {
                rigidbody->BounceOff(other->GetNormal(gameObject->transform.position));
            }
        }
    }

    void Update() {
        if (currentState == FREE) {
            if (rigidbody->velocity.Magnitude() > maxSpeed) {
                rigidbody->velocity = rigidbody->velocity.Normalize() * maxSpeed;
            }
        } else if (currentState == BINDED) {
            if (lastBindedBy != nullptr) {
                Rigidbody2D *boundRigidbody = lastBindedBy->GetComponent<Rigidbody2D>();
                CircleCollider2D *boundCollider = lastBindedBy->GetComponent<CircleCollider2D>();

                if (boundRigidbody->velocity.Magnitude() > 0.1f) {
                    lastBindedVelocity = boundRigidbody->velocity;
                }

                gameObject->transform.position = lastBindedBy->transform.position + lastBindedVelocity.Normalize() * boundCollider->radius;
            }
        } else if (currentState == KICKED) {
            if (rigidbody->velocity.Magnitude() < kickedStateMinSpeed) {
                currentState = FREE;
            }
        }
    }

    void Kick(Vector2 direction, float force, GameObject *kicker) {
        if (currentState == BINDED) {
            rigidbody->AddForce(direction * force);
            currentState = KICKED;
            lastKickedBy = kicker;
        }
    }

    void Bind(GameObject *binder) {
        if (SDL_GetTicks() - lastBindTime < bindCooldown) return;
        if (lastBindedBy != nullptr){
            lastBindedBy->GetComponent<Collider2D>()->enabled = true;
        }


        lastBindTime = SDL_GetTicks();
        currentState = BINDED;
        lastBindedBy = binder;
        lastBindedBy->GetComponent<Collider2D>()->enabled = false;

        gameObject->GetComponent<VelocityToAnimSpeedController>()->SetBackupRigidbody(binder->GetComponent<Rigidbody2D>());
    }

    void Draw() {}

    Component *Clone(GameObject *parent) {
        BallStateMachine *newBallStateMachine = new BallStateMachine(parent, kickedStateMinSpeed, bindCooldown);
        return newBallStateMachine;
    }

private:
    State currentState;
    Rigidbody2D *rigidbody;
};

// AI Control
// Auto take over the control of the object if movement controller is not enabled
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
    AIController(GameObject *parent, GameObject *target, float speed, bool isTeam1) : Component(parent) {
        this->rigidbody = this->gameObject->GetComponent<Rigidbody2D>();
        this->movementController = this->gameObject->GetComponent<MovementController>();
        this->target = target;
        this->speed = speed;

        this->isTeam1 = isTeam1;
    }

    virtual void Update() = 0;

    void Draw() {}

    virtual Component *Clone(GameObject *parent) = 0;
};

class AIGoalKeeper : public AIController {
private:
    float alertZoneYStart = 0, alertZoneYEnd = 0;

public:
    AIGoalKeeper(GameObject *parent, GameObject *target, float speed, bool isTeam1) : AIController(parent, target, speed, isTeam1) {
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

    void Update() {
        if (rigidbody == nullptr)
            return;
        if (movementController != nullptr && movementController->GetEnabled())
            return;

        float actualSpeed = speed * 1 / FPS;

        Vector2 targetPosition = target->transform.position;
        Vector2 currentPosition = gameObject->transform.position;

        // Target is in the alert zone
        if (targetPosition.x >= alertZoneXStart && targetPosition.x <= alertZoneXEnd) {
            if (targetPosition.y < currentPosition.y) {
                rigidbody->AddForce(Vector2(0, -1).Normalize() * actualSpeed);
            } else if (targetPosition.y > currentPosition.y) {
                rigidbody->AddForce(Vector2(0, 1).Normalize() * actualSpeed);
            }
        }

        // Target is in the danger zone
        else if (targetPosition.x >= dangerZoneXStart && targetPosition.x <= dangerZoneXEnd &&
                 targetPosition.y >= alertZoneYStart && targetPosition.y <= alertZoneYEnd) {
            Vector2 direction = (targetPosition - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }

        // Target is neither, restore original position
        else {
            Vector2 dangerZoneCenter((dangerZoneXStart + dangerZoneXEnd) / 2, (alertZoneYStart + alertZoneYEnd) / 2);
            Vector2 direction = (dangerZoneCenter - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }
    }

    Component *Clone(GameObject *parent) {
        AIGoalKeeper *newAIGoalKeeper = new AIGoalKeeper(parent, target, speed, isTeam1);
        return newAIGoalKeeper;
    }
};

class AIDefender : public AIController {
public:
    AIDefender(GameObject *parent, GameObject *target, float speed, bool isTeam1) : AIController(parent, target, speed, isTeam1) {
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

    void Update() {
        if (rigidbody == nullptr)
            return;
        if (movementController != nullptr && movementController->GetEnabled())
            return;

        float actualSpeed = speed * 1 / FPS;

        Vector2 targetPosition = target->transform.position;
        Vector2 currentPosition = gameObject->transform.position;

        // Target is in the alert zone
        if (targetPosition.x >= alertZoneXStart && targetPosition.x <= alertZoneXEnd) {
            Vector2 direction = (targetPosition - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }

        // Target is in the danger zone
        else if (targetPosition.x >= dangerZoneXStart && targetPosition.x <= dangerZoneXEnd) {
            Vector2 direction = (targetPosition - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }

        // Target is neither, restore original position
        else {
            Vector2 dangerZoneCenter((dangerZoneXStart + dangerZoneXEnd) / 2, currentPosition.y);
            Vector2 direction = (dangerZoneCenter - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }
    }

    Component *Clone(GameObject *parent) {
        AIDefender *newAIDefender = new AIDefender(parent, target, speed, isTeam1);
        return newAIDefender;
    }
};

class AIAttacker : public AIController {
public:
    AIAttacker(GameObject *parent, GameObject *target, float speed, bool isTeam1) : AIController(parent, target, speed, isTeam1) {
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

    void Update() {
        if (rigidbody == nullptr)
            return;
        if (movementController != nullptr && movementController->GetEnabled())
            return;

        float actualSpeed = speed * 1 / FPS;

        Vector2 targetPosition = target->transform.position;
        Vector2 currentPosition = gameObject->transform.position;

        // Target is in the alert zone
        if (targetPosition.x >= alertZoneXStart && targetPosition.x <= alertZoneXEnd) {
            Vector2 direction = (targetPosition - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }

        // Target is in the danger zone
        else if (targetPosition.x >= dangerZoneXStart && targetPosition.x <= dangerZoneXEnd) {
            Vector2 direction = (targetPosition - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }

        // Target is neither, restore original position
        else {
            Vector2 dangerZoneCenter((dangerZoneXStart + dangerZoneXEnd) / 2, currentPosition.y);
            Vector2 direction = (dangerZoneCenter - currentPosition).Normalize();
            rigidbody->AddForce(direction * actualSpeed);
        }
    }

    Component *Clone(GameObject *parent) {
        AIAttacker *newAIAttacker = new AIAttacker(parent, target, speed, isTeam1);
        return newAIAttacker;
    }
};

#endif // AI_HPP