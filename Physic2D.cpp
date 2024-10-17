#include "Physic2D.hpp"

CollisionManager *CollisionManager::instance = nullptr;

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

    const float MINIMUM_VELOCITY = 0.05f;

    if (this->velocity.Magnitude() < MINIMUM_VELOCITY) {
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

    this->acceleration = Vector2(0, 0);

    // std::cout << "Bouncing off" << std::endl;
    // std::cout << "Velocity before: " << this->velocity.x << ", " << this->velocity.y << std::endl;
    this->velocity = Reflect(this->velocity, normal) * this->bounciness;
    // std::cout << "Velocity after: " << this->velocity.x << ", " << this->velocity.y << std::endl;
}

Vector2 Rigidbody2D::Reflect(Vector2 velocity, Vector2 normal) {
    if (normal.Magnitude() < EPS) {
        return Vector2(0, 0);
    }
    return velocity - 2 * (velocity.Dot(normal)) * normal;
}

Component *Rigidbody2D::Clone(GameObject *parent) {
    Rigidbody2D *newRigidbody = new Rigidbody2D(parent, this->mass, this->drag, this->bounciness);
    return newRigidbody;
}

#pragma endregion

#pragma region Collider2D

// Collider2D Implementation
Collider2D::Collider2D(GameObject *parent, Vector2 offset) : Component(parent) {
    this->offset = offset;
    CollisionManager::GetInstance()->AddCollider(this);
}

Collider2D::~Collider2D() {
    CollisionManager::GetInstance()->RemoveCollider(this);
}

void Collider2D::Update() {}
void Collider2D::Draw() {}

void Collider2D::SetOffset(Vector2 offset) {
    this->offset = offset;
}

// CollisionManager Implementation
CollisionManager::CollisionManager() {}

CollisionManager *CollisionManager::GetInstance() {
    if (instance == nullptr) {
        instance = new CollisionManager();
    }
    return instance;
}

void CollisionManager::AddCollider(Collider2D *collider) {
    this->colliders.push_back(collider);
}

void CollisionManager::RemoveCollider(Collider2D *collider) {
    for (int i = 0; i < this->colliders.size(); i++) {
        if (this->colliders[i]->gameObject->GetName() == collider->gameObject->GetName()) {
            this->colliders.erase(this->colliders.begin() + i);
            return;
        }
    }
}

void CollisionManager::Update() {
    for (auto &collider1 : this->colliders) {
        for (auto &collider2 : this->colliders) {
            if (collider1->gameObject->GetName() == collider2->gameObject->GetName()) {
                continue;
            }

            if (collider1->CheckCollision(collider2)) {
                collider1->OnCollisionEnter.raise(collider2);
                collider2->OnCollisionEnter.raise(collider1);
            }
        }
    }
}


// CircleCollider2D Implementation
CircleCollider2D::CircleCollider2D(GameObject *parent, Vector2 offset, float radius) : Collider2D(parent, offset) {
    this->radius = radius;
}

CircleCollider2D::~CircleCollider2D() {}

void CircleCollider2D::SetRadius(float radius){
    this->radius = radius;
}

Component *CircleCollider2D::Clone(GameObject *parent) {
    CircleCollider2D *newCollider = new CircleCollider2D(parent, this->offset, this->radius);
    return newCollider;
}

bool CircleCollider2D::CheckCollision(Collider2D *other) {
    if (dynamic_cast<CircleCollider2D *>(other)) {
        return this->CheckCollision(dynamic_cast<CircleCollider2D *>(other));
    } else if (dynamic_cast<BoxCollider2D *>(other)) {
        return this->CheckCollision(dynamic_cast<BoxCollider2D *>(other));
    }
}

bool CircleCollider2D::CheckCollision(CircleCollider2D *other) {
    return (this->gameObject->transform.position - other->gameObject->transform.position).Magnitude() < this->radius + other->radius;
}

bool CircleCollider2D::CheckCollision(BoxCollider2D *other) {
    return ::CheckCollision(this, other);
}

Vector2 CircleCollider2D::GetNormal(Vector2 point) {
    return (point - this->gameObject->transform.position).Normalize();
}

// BoxCollider2D Implementation
BoxCollider2D::BoxCollider2D(GameObject *parent, Vector2 offset, Vector2 size) : Collider2D(parent, offset) {
    this->size = size;
}

BoxCollider2D::~BoxCollider2D() {}

void BoxCollider2D::SetSize(Vector2 size) {
    this->size = size;
}

Component *BoxCollider2D::Clone(GameObject *parent) {
    BoxCollider2D *newCollider = new BoxCollider2D(parent, this->offset, this->size);
    return newCollider;
}

bool BoxCollider2D::CheckCollision(Collider2D *other) {

    if (dynamic_cast<CircleCollider2D *>(other)) {
        return this->CheckCollision(dynamic_cast<CircleCollider2D *>(other));
    } else if (dynamic_cast<BoxCollider2D *>(other)) {
        return this->CheckCollision(dynamic_cast<BoxCollider2D *>(other));
    }
}

bool BoxCollider2D::CheckCollision(CircleCollider2D *other) {
    return ::CheckCollision(other, this);
}

bool BoxCollider2D::CheckCollision(BoxCollider2D *other) {
    Vector2 box1Min = this->gameObject->transform.position - this->size / 2 + this->offset;
    Vector2 box1Max = this->gameObject->transform.position + this->size / 2 + this->offset;
    Vector2 box2Min = other->gameObject->transform.position - other->size / 2 + other->offset;
    Vector2 box2Max = other->gameObject->transform.position + other->size / 2 + other->offset;

    bool collisionX = box1Max.x >= box2Min.x && box1Min.x <= box2Max.x;
    bool collisionY = box1Max.y >= box2Min.y && box1Min.y <= box2Max.y;

    return collisionX && collisionY;
}

Vector2 BoxCollider2D::GetNormal(Vector2 point) {
    Vector2 boxMin = this->gameObject->transform.position - this->size / 2 + this->offset;
    Vector2 boxMax = this->gameObject->transform.position + this->size / 2 + this->offset;

    if (point.x < boxMin.x) {
        return Vector2(-1, 0);
    }
    if (point.x > boxMax.x) {
        return Vector2(1, 0);
    }
    if (point.y < boxMin.y) {
        return Vector2(0, -1);
    }
    if (point.y > boxMax.y) {
        return Vector2(0, 1);
    }

    // Determine the normal based on the box center
    Vector2 boxCenter = (boxMin + boxMax) / 2;
    Vector2 direction = (point - boxCenter).Normalize();

    // return direction;

    if (direction.x * direction.x > direction.y * direction.y) {
        return Vector2(direction.x > 0 ? 1 : -1, 0);
    } else {
        return Vector2(0, direction.y > 0 ? 1 : -1);
    }

    return Vector2(0, 0);
}

//General Collision Functions
bool CheckCollision(CircleCollider2D *circle, BoxCollider2D *box) {
    // Find the closest point on the box to the circle
    float closestX = std::max(box->gameObject->transform.position.x - box->size.x / 2, std::min(circle->gameObject->transform.position.x, box->gameObject->transform.position.x + box->size.x / 2));
    float closestY = std::max(box->gameObject->transform.position.y - box->size.y / 2, std::min(circle->gameObject->transform.position.y, box->gameObject->transform.position.y + box->size.y / 2));

    // Calculate the distance between the circle's center and this closest point
    float distanceX = circle->gameObject->transform.position.x - closestX;
    float distanceY = circle->gameObject->transform.position.y - closestY;

    // If the distance is less than the circle's radius, an intersection occurs
    float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
    return distanceSquared < (circle->radius * circle->radius);
}


#pragma endregion