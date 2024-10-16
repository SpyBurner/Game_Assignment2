#include "Physic2D.hpp"

#pragma region Rigidbody2D

Rigidbody2D::Rigidbody2D(GameObject *parent, float mass, float drag) : Component(parent) {
    this->mass = mass;
    this->velocity = Vector2(0, 0);
    this->acceleration = Vector2(0, 0);
    this->drag = 0.0f;
}

Rigidbody2D::~Rigidbody2D() {}

void Rigidbody2D::Update() {
    this->acceleration = this->acceleration * drag * -1;
    this->velocity += this->acceleration;

    this->velocity = this->velocity * drag * -1;

    this->gameObject->transform.position += this->velocity;
}

void Rigidbody2D::Draw() {}

void Rigidbody2D::AddForce(Vector2 force) {
    this->acceleration += force / this->mass;
}

void Rigidbody2D::SetDrag(float drag) {
    this->drag = drag;
}

Component *Rigidbody2D::Clone(GameObject *parent) {
    Rigidbody2D *newRigidbody = new Rigidbody2D(parent, this->mass, this->drag);
    return newRigidbody;
}

#pragma endregion
