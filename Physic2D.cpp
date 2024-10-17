#include "Physic2D.hpp"

#pragma region Rigidbody2D

Rigidbody2D::Rigidbody2D(GameObject *parent, float mass, float drag, float bounciness) : Component(parent) {
    this->mass = mass;
    this->velocity = Vector2(0, 0);
    this->acceleration = Vector2(0, 0);
    this->drag = drag;
    this->bounciness = bounciness;
}

Rigidbody2D::~Rigidbody2D() {}

void Rigidbody2D::Update() {
    this->velocity += this->acceleration;
    this->acceleration = Vector2(0, 0);

    this->velocity = this->velocity * (1 - this->drag);

    if (this->velocity.Magnitude() < EPS) {
        this->velocity = Vector2(0, 0);
    }

    this->gameObject->transform.position += this->velocity;
}

void Rigidbody2D::Draw() {}

void Rigidbody2D::AddForce(Vector2 force) {
    this->acceleration += force / this->mass;
}

void Rigidbody2D::SetDrag(float drag) {
    this->drag = drag;
}

void Rigidbody2D::SetBounciness(float bounciness) {
    this->bounciness = bounciness;
}

void Rigidbody2D::BounceOff(Vector2 normal) {
    if (Vector2::Dot(this->velocity, normal) > 0) {
        return;
    }
    
    this->velocity = Reflect(this->velocity, normal) * this->bounciness;
}

Vector2 Rigidbody2D::Reflect(Vector2 velocity, Vector2 normal){
    return velocity - 2 * (velocity.Dot(normal)) * normal;
}

Component *Rigidbody2D::Clone(GameObject *parent) {
    Rigidbody2D *newRigidbody = new Rigidbody2D(parent, this->mass, this->drag, this->bounciness);
    return newRigidbody;
}

#pragma endregion
