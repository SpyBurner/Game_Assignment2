#ifndef PHYSIC2D_HPP
#define PHYSIC2D_HPP

#include "Global.hpp"
#include "CustomClasses.hpp"

class Rigidbody2D : public Component {
private:
    Vector2 acceleration;
    float mass, drag, bounciness;

public:
    Vector2 velocity;
    
    Rigidbody2D(GameObject *parent, float mass, float drag, float bounciness);
    ~Rigidbody2D();
    
    void Update();
    void Draw();
    void AddForce(Vector2 force);

    void SetDrag(float drag);
    void SetBounciness(float bounciness);

    void BounceOff(Vector2 normal);
    
    static Vector2 Reflect(Vector2 velocity, Vector2 normal);

    Component *Clone(GameObject *parent);
};

class Collider2D : public Component {
public:
    Event OnCollisionEnter();

    Collider2D(GameObject *parent);
    ~Collider2D();

    void Update();
    void Draw();
};

#endif