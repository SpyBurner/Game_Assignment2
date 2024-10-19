#ifndef HELPER_HPP
#define HELPER_HPP

#include "Global.hpp"
#include "CustomClasses.hpp"
#include "Physic2D.hpp"

class RollSpeedController : public Component {
private:
    Rigidbody2D *rigidbody;
    Animator *animator;

public:
    RollSpeedController(GameObject *parent) : Component(parent) {
        this->rigidbody = this->gameObject->GetComponent<Rigidbody2D>();
        this->animator = this->gameObject->GetComponent<Animator>();
    }

    ~RollSpeedController() {}

    void Update() {
        if (this->rigidbody->velocity.Magnitude() > 0.0){
            if (animator->GetCurrentClip()->isPlaying == false)
                animator->Play("Roll");
            animator->GetCurrentClip()->speedScale = this->rigidbody->velocity.Magnitude();
        }
        else{
            animator->GetCurrentClip()->speedScale = 1.0;
            animator->Stop();
        }
    }

    void Draw() {}

    Component *Clone(GameObject *parent) {
        RollSpeedController *newRollSpeedController = new RollSpeedController(parent);
        return newRollSpeedController;
    }
};

class StayInBounds : public Component {
public:
    bool teleport = false;
    StayInBounds(GameObject *parent, bool teleport) : Component(parent) {
        this->teleport = teleport;
    }

    ~StayInBounds() {}

    void Update() {
        Rigidbody2D *rigidbody = this->gameObject->GetComponent<Rigidbody2D>();
        if (this->gameObject->transform.position.x < 0) {
            if (teleport){
                this->gameObject->transform.position.x = WIDTH;
            }
            else{
                if (rigidbody != nullptr)
                    rigidbody->BounceOff(Vector2(1, 0));
                else
                    this->gameObject->transform.position.x = 0;
            }
        }
        if (this->gameObject->transform.position.x > WIDTH) {
            if (teleport){
                this->gameObject->transform.position.x = 0;
            }
            else{
                if (rigidbody != nullptr)
                    rigidbody->BounceOff(Vector2(-1, 0));
                else
                    this->gameObject->transform.position.x = WIDTH;
            }
        }
        if (this->gameObject->transform.position.y < 0) {
            if (teleport){
                this->gameObject->transform.position.y = HEIGHT;
            }
            else{
                if (rigidbody != nullptr)
                    rigidbody->BounceOff(Vector2(0, 1));
                else
                    this->gameObject->transform.position.y = 0;
            }
        }
        if (this->gameObject->transform.position.y > HEIGHT - 44) {
            if (teleport){
                this->gameObject->transform.position.y = 0;
            }
            else{
                if (rigidbody != nullptr)
                    rigidbody->BounceOff(Vector2(0, -1));
                else
                    this->gameObject->transform.position.y = HEIGHT - 44;
            }
        }
    
    }

    void Draw() {}

    Component *Clone(GameObject *parent) {
        StayInBounds *newStayInBounds = new StayInBounds(parent, this->teleport);
        return newStayInBounds;
    }
};



#endif // HELPER_HPP