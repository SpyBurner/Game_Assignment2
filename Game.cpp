#include "Game.hpp"
#include "CustomClasses.hpp"
#include "Global.hpp"
#include "Helper.hpp"
#include "Components.hpp"
#include "Physic2D.hpp"

#include <cmath>
#include <iostream>

SDL_Event Game::event;

Game::Game() {
    isRunning = false;
}

Game::~Game() {
}

void Game::init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        std::cout << "Subsystems Initialised..." << std::endl;

        window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if (window) {
            std::cout << "Window created..." << std::endl;
        }

        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            std::cout << "Renderer created..." << std::endl;
        }

        RENDERER = renderer;

        isRunning = true;
    } else {
        isRunning = false;
    }

    objectInit();
}

GameObject *player = new GameObject("Player");

void Game::objectInit() {
    // Add your object initialisation here

    std::cout << "Object Initialisation..." << std::endl;

    Scene *mainScene = new Scene("Main");


#pragma region TEST ANIMATOR
// // Test scene

// player->transform.position = Vector2(150, 200);
// player->transform.scale = Vector2(5, 5);
// player->AddComponent(new SpriteRenderer(player, renderer, Vector2(35, 44)));

// SpriteRenderer *spriteRenderer = player->GetComponent<SpriteRenderer>();

// AnimationClip playerIdle = AnimationClip("Idle", "Assets/kirby_fall.png", Vector2(35, 44), 2000, false, 1.0, 0, 14);
// AnimationClip playerFloat = AnimationClip("Float", "Assets/kirby_float.png", Vector2(35, 44), 2000, false, 1.0, 0, 3);

// player->AddComponent(new Animator(player, {playerIdle, playerFloat}));

#pragma endregion

#pragma region TEST PHYSIC2D
    GameObject *ball = new GameObject("Ball");
    ball->transform.position = Vector2(150, 320);
    ball->transform.scale = Vector2(3, 3);

    ball->AddComponent(new SpriteRenderer(ball, Vector2(15, 15), LoadSpriteSheet("Assets/default.png")));

    ball->AddComponent(new Animator(ball, {AnimationClip("Roll", "Assets/soccer_ball.png", Vector2(15, 15), 1000, true, 1.0, 0, 1)}));
    ball->GetComponent<Animator>()->Play("Roll");

    ball->AddComponent(new Rigidbody2D(ball, 1, 0.025, 0.95));
    ball->GetComponent<Rigidbody2D>()->AddForce(Vector2(70, 70));

    ball->AddComponent(new RollSpeedController(ball));
    ball->AddComponent(new StayInBounds(ball, false));

    ball->AddComponent(new CircleCollider2D(ball, Vector2(0, 0), 7.5));
    // ball->GetComponent<CircleCollider2D>()->OnCollisionEnter.addHandler(
    //     [ball](Collider2D *collider) {
    //         Rigidbody2D *rigidbody = ball->GetComponent<Rigidbody2D>();
    //         rigidbody->BounceOff(collider->GetNormal(ball->transform.position));
    //     }
    // );

    mainScene->AddGameObject(ball);

    // // Instantiate 4 walls around the border of the screen

    // // Another wall (example: middle wall)
    // GameObject *middleWall = new GameObject("MiddleWall");
    // middleWall->transform.position = Vector2(640, 360);
    // middleWall->transform.scale = Vector2(10, 50);

    // middleWall->AddComponent(new SpriteRenderer(middleWall, Vector2(15, 30), LoadSpriteSheet("Assets/wall.png")));
    // middleWall->AddComponent(new BoxCollider2D(middleWall, Vector2(0, 0),
    //  Vector2(middleWall->transform.scale.x * 15, middleWall->transform.scale.y * 30)
    //  ));
    // mainScene->AddGameObject(middleWall);

    // // Left wall
    // GameObject *leftWall = new GameObject("LeftWall");
    // leftWall->transform.position = Vector2(0, 360);
    // leftWall->transform.scale = Vector2(10, 720);

    // leftWall->AddComponent(new SpriteRenderer(leftWall, Vector2(15, 30), LoadSpriteSheet("Assets/wall.png")));
    // leftWall->AddComponent(new BoxCollider2D(leftWall, Vector2(0, 0),
    //  Vector2(leftWall->transform.scale.x * 15, leftWall->transform.scale.y * 30)
    //  ));
    // mainScene->AddGameObject(leftWall);
#pragma endregion

#pragma region TEST CONTROLLER

    GameObject *player = new GameObject("Player");
    player->transform.position = Vector2(100, 360);
    player->transform.scale = Vector2(2, 2);

    player->AddComponent(new SpriteRenderer(player, Vector2(31, 82), LoadSpriteSheet("Assets/actor.png")));
    player->AddComponent(new Rigidbody2D(player, 1, 0.025, .5));

    player->AddComponent(new BoxCollider2D(player, Vector2(0, 0), 
        Vector2(31 * player->transform.scale.x, 82 * player->transform.scale.y)));

    player->AddComponent(new StayInBounds(player, false));

    GameObject *player2 = GameObject::Instantiate("Player2", player, Vector2(600, 360), 0, Vector2(2, 2));

    player->GetComponent<BoxCollider2D>()->OnCollisionEnter.addHandler( 
        [player](Collider2D *collider) {
            Rigidbody2D *rigidbody = player->GetComponent<Rigidbody2D>();

            if (collider->gameObject->GetName() == "Ball") {
                collider->gameObject->GetComponent<Rigidbody2D>()->AddForce(rigidbody->velocity);
                return;
            }

            rigidbody->BounceOff(collider->GetNormal(player->transform.position));
        }
    );

    player2->GetComponent<BoxCollider2D>()->OnCollisionEnter.addHandler(
        [player2](Collider2D *collider) {
            Rigidbody2D *rigidbody = player2->GetComponent<Rigidbody2D>();

            if (collider->gameObject->GetName() == "Ball") {
                collider->gameObject->GetComponent<Rigidbody2D>()->AddForce(rigidbody->velocity);
                return;
            }

            rigidbody->BounceOff(collider->GetNormal(player2->transform.position));
        }
    );

    player->AddComponent(new MovementController(player, 10, true));
    player2->AddComponent(new MovementController(player2, 10, false));

    mainScene->AddGameObject(player);
    mainScene->AddGameObject(player2);

#pragma endregion

    SceneManager::GetInstance()->AddScene(mainScene);
    SceneManager::GetInstance()->LoadScene("Main");
}

void Game::handleEvents() {
    
    SDL_PollEvent(&Game::event);
    switch (event.type) {
    case SDL_QUIT:
        isRunning = false;
        break;
    default:
        break;
    }
}

float lastSpawnTime = -3000;
float spawnCooldown = 3000;
int spawnCount = 0;

GameObject *previousObject = player;

void Game::update() {
    // Game logic updates go here

#pragma region TEST ANIMATOR
// TEST INSTANTIATE
// if (SDL_GetTicks() - lastSpawnTime >= spawnCooldown) {
//     GameObject *newObject = GameObject::Instantiate("Player" + std::to_string(spawnCount),
//     *player, previousObject->transform.position + Vector2(30, 0), 0, Vector2(5, 5));
//     lastSpawnTime = SDL_GetTicks();
//     previousObject = newObject;
//     spawnCount++;

//     newObject->GetComponent<Animator>()->GetClip("Idle")->onComplete->addHandler(
//         [newObject]() {
//             newObject->GetComponent<Animator>()->Play("Float");
//         }
//     );

//     newObject->GetComponent<Animator>()->GetClip("Float")->onComplete->addHandler(
//         [newObject]() {
//             newObject->GetComponent<Animator>()->Play("Idle");
//         }
//     );

//     newObject->GetComponent<Animator>()->Play("Idle");
// }
//
#pragma endregion

#pragma region TEST PHYSIC2D
    // GameObject *ball = SceneManager::GetInstance()->GetGameObject("Ball");

    // Vector2 pos = ball->transform.position;
    // Rigidbody2D *rigidbody = ball->GetComponent<Rigidbody2D>();
    // // rigidbody->AddForce(Vector2(sin(SDL_GetTicks() / 1000.0), sin(SDL_GetTicks() / 1000.0 + 1 / 2 * 3.14)).Normalize() * 10);
    // // rigidbody->AddForce(Vector2(2, 0));

    // // std::cout << "Ball Position: " << pos.x << ", " << pos.y << std::endl;
#pragma endregion

    SceneManager::GetInstance()->Update();
}

void Game::render() {
    SDL_RenderClear(renderer);
    // Add your rend☺ering stuff here

    SceneManager::GetInstance()->Draw();
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    delete SceneManager::GetInstance();

    for (auto &texture : TEXTURES) {
        SDL_DestroyTexture(texture);
    }
    TEXTURES.clear();

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    std::cout << "Game cleaned..." << std::endl;
}

bool Game::running() {
    return isRunning;
}