#ifndef HELPER_HPP
#define HELPER_HPP

#include "Global.hpp"
#include "CustomClasses.hpp"
#include "Physic2D.hpp"

class VelocityToAnimSpeedController : public Component {
private:
    Rigidbody2D *rigidbody = nullptr;
    Animator *animator = nullptr;
    std::string animName;

public:
    VelocityToAnimSpeedController(GameObject *parent, std::string animName) : Component(parent) {
        this->animName = animName;
    }

    ~VelocityToAnimSpeedController() {}

    void Update() {
        if (this->rigidbody == nullptr || this->animator == nullptr){
            this->rigidbody = this->gameObject->GetComponent<Rigidbody2D>();
            this->animator = this->gameObject->GetComponent<Animator>();
        }

        if (this->rigidbody == nullptr || this->animator == nullptr) return;

        if (this->rigidbody->velocity.Magnitude() > 0.0){
            if (animator->GetClip(animName)->isPlaying == false)
                animator->Play(animName);
            animator->GetCurrentClip()->speedScale = this->rigidbody->velocity.Magnitude();
        }
        else{
            animator->GetCurrentClip()->speedScale = 1.0;
            animator->Stop();
        }
    }

    void Draw() {}

    Component *Clone(GameObject *parent) {
        VelocityToAnimSpeedController *newRollSpeedController = new VelocityToAnimSpeedController(parent, this->animName);
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