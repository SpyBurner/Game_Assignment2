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
        if (this->gameObject->transform.position.x < 0) {
            this->gameObject->transform.position.x = (teleport) ? WIDTH - 35 : 0;
        }
        if (this->gameObject->transform.position.x > WIDTH - 35) {
            this->gameObject->transform.position.x = (teleport) ? 0 : WIDTH - 35;
        }
        if (this->gameObject->transform.position.y < 0) {
            this->gameObject->transform.position.y = (teleport) ? HEIGHT - 44 : 0;
        }
        if (this->gameObject->transform.position.y > HEIGHT - 44) {
            this->gameObject->transform.position.y = (teleport) ? 0 : HEIGHT - 44;
        }
    }

    void Draw() {}

    Component *Clone(GameObject *parent) {
        StayInBounds *newStayInBounds = new StayInBounds(parent, this->teleport);
        return newStayInBounds;
    }
};

#endif // HELPER_HPP