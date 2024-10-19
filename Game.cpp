#include "CustomClasses.hpp"
#include "Components.hpp"
#include "Game.hpp"
#include "Global.hpp"
#include "Physic2D.hpp"
#include "Helper.hpp"

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

#pragma region Background Setup
    GameObject *background = new GameObject("Background");
    background->transform.position = Vector2(640, 360);
    background->transform.scale = Vector2(1, 1);

    background->AddComponent(new SpriteRenderer(background, Vector2(1280, 720), -10, LoadSpriteSheet("Assets/Sprites/yard.png")));

    mainScene->AddGameObject(background);
#pragma endregion

#pragma region Ball Setup
    GameObject *ball = new GameObject("Ball");
    ball->tag = "Ball";
    ball->transform.position = Vector2(640, 100);
    ball->transform.scale = Vector2(2, 2);

    ball->AddComponent(new SpriteRenderer(ball, Vector2(15, 15), 10, LoadSpriteSheet("Assets/default.png")));

    ball->AddComponent(new Animator(ball, {AnimationClip("Roll", "Assets/soccer_ball.png", Vector2(15, 15), 1000, true, 1.0, 0, 1)}));
    ball->GetComponent<Animator>()->Play("Roll");

    ball->AddComponent(new Rigidbody2D(ball, 1, 0.025, .9));

    ball->AddComponent(new VelocityToAnimSpeedController(ball, "Roll"));
    ball->AddComponent(new StayInBounds(ball, false));

    ball->AddComponent(new CircleCollider2D(ball, Vector2(0, 0), 7.5));

    ball->AddComponent(new BallStateMachine(ball, 2.0, 700, 100));

    ball->GetComponent<CircleCollider2D>()->OnCollisionEnter.addHandler(
        [ball](Collider2D *collider) {
            ball->GetComponent<BallStateMachine>()->OnCollisionEnter(collider);
        }
    );

    mainScene->AddGameObject(ball);
#pragma endregion

#pragma region Player setup

    GameObject *player1 = new GameObject("Player1");
    player1->transform.scale = Vector2(2, 2);

    player1->AddComponent(new SpriteRenderer(player1, Vector2(31, 82), 0, LoadSpriteSheet("Assets/actor.png")));
    player1->AddComponent(new Rigidbody2D(player1, 1, 0.04, .2));
    player1->AddComponent(new CircleCollider2D(player1, Vector2(0, 0), 17 * player1->transform.scale.x));
    player1->AddComponent(new StayInBounds(player1, false));
    player1->AddComponent(new RotateTowardVelocity(player1, Vector2(0, -1)));
    player1->AddComponent(new VelocityToAnimSpeedController(player1, "Run"));
    
    player1->transform.position = Vector2(25, 360);
        GameObject *player2 = GameObject::Instantiate("Player2", player1, Vector2(100, 420), 0, Vector2(2, 2));
        GameObject *player3 = GameObject::Instantiate("Player3", player1, Vector2(100, 300), 0, Vector2(2, 2));

        GameObject *player4 = GameObject::Instantiate("Player4", player1, Vector2(1125, 300), 0, Vector2(2, 2));
        GameObject *player5 = GameObject::Instantiate("Player5", player1, Vector2(1125, 360), 0, Vector2(2, 2));
    GameObject *player6 = GameObject::Instantiate("Player6", player1, Vector2(1200, 420), 0, Vector2(2, 2));

    player1->tag = player2->tag = player3->tag = "1";
    player4->tag = player5->tag = player6->tag = "2";

    player1->AddComponent(new Animator(player1, {AnimationClip("Run", "Assets/Sprites/football.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));
    player2->AddComponent(new Animator(player2, {AnimationClip("Run", "Assets/Sprites/football2.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));
    player3->AddComponent(new Animator(player3, {AnimationClip("Run", "Assets/Sprites/football3.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));
    player4->AddComponent(new Animator(player4, {AnimationClip("Run", "Assets/Sprites/football4.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));
    player5->AddComponent(new Animator(player5, {AnimationClip("Run", "Assets/Sprites/football5.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));
    player6->AddComponent(new Animator(player6, {AnimationClip("Run", "Assets/Sprites/football6.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));

    auto setupCollisionHandler = [](GameObject *player) {
        player->GetComponent<CircleCollider2D>()->OnCollisionEnter.addHandler(
            [player](Collider2D *collider) {
                Rigidbody2D *rigidbody = player->GetComponent<Rigidbody2D>();
                if (collider->gameObject->tag == "Wall"){
                    rigidbody->BounceOff(collider->GetNormal(player->transform.position));
                }
            }
        );
    };

    setupCollisionHandler(player1);
    setupCollisionHandler(player2);
    setupCollisionHandler(player3);
    setupCollisionHandler(player4);
    setupCollisionHandler(player5);
    setupCollisionHandler(player6);

    player1->AddComponent(new MovementController(player1, 10, true));
    player2->AddComponent(new MovementController(player2, 10, true));
    player3->AddComponent(new MovementController(player3, 10, true));

    player4->AddComponent(new MovementController(player4, 10, false));
    player5->AddComponent(new MovementController(player5, 10, false));
    player6->AddComponent(new MovementController(player6, 10, false));

    player1->AddComponent(new KickControl(player1, ball, SDLK_SPACE, 17));
    player2->AddComponent(new KickControl(player2, ball, SDLK_SPACE, 12));
    player3->AddComponent(new KickControl(player3, ball, SDLK_SPACE, 12));

    player4->AddComponent(new KickControl(player4, ball, SDLK_KP_ENTER, 12));
    player5->AddComponent(new KickControl(player5, ball, SDLK_KP_ENTER, 12));
    player6->AddComponent(new KickControl(player6, ball, SDLK_KP_ENTER, 17));

    player1->AddComponent(new AIGoalKeeper(player1, ball, 10, true));
    player2->AddComponent(new AIDefender(player2, ball, 10, true));
    player3->AddComponent(new AIAttacker(player3, ball, 10, true));

    player4->AddComponent(new AIAttacker(player6, ball, 10, false));
    player5->AddComponent(new AIDefender(player5, ball, 10, false));
    player6->AddComponent(new AIGoalKeeper(player6, ball, 10, false));

    // First controller switcher for player1, player2, and player3
    GameObject *controllerSwitcher1 = new GameObject("ControllerSwitcher1");
    TeamControl* movementControllerSwitcher1 = dynamic_cast<TeamControl *>(controllerSwitcher1->AddComponent(
        new TeamControl(controllerSwitcher1, LoadSpriteSheet("Assets/blue_indicator.png"), 75.0)
    ));
    movementControllerSwitcher1->AddMovementController(SDLK_1, player1->GetComponent<MovementController>());
    movementControllerSwitcher1->AddMovementController(SDLK_2, player2->GetComponent<MovementController>());
    movementControllerSwitcher1->AddMovementController(SDLK_3, player3->GetComponent<MovementController>());
    mainScene->AddGameObject(controllerSwitcher1);

    // Second controller switcher for player4, player5, and player6
    GameObject *controllerSwitcher2 = new GameObject("ControllerSwitcher2");
    TeamControl* movementControllerSwitcher2 = dynamic_cast<TeamControl *>(controllerSwitcher2->AddComponent(
        new TeamControl(controllerSwitcher2, LoadSpriteSheet("Assets/red_indicator.png"), 75.0)
    ));
    movementControllerSwitcher2->AddMovementController(SDLK_KP_4, player4->GetComponent<MovementController>());
    movementControllerSwitcher2->AddMovementController(SDLK_KP_5, player5->GetComponent<MovementController>());
    movementControllerSwitcher2->AddMovementController(SDLK_KP_6, player6->GetComponent<MovementController>());
    mainScene->AddGameObject(controllerSwitcher2);

    //Test
    movementControllerSwitcher2->AddMovementController(SDLK_KP_7, player2->GetComponent<MovementController>());
    //

    mainScene->AddGameObject(player1);
    // mainScene->AddGameObject(player2);
    // mainScene->AddGameObject(player3);
    mainScene->AddGameObject(player4);
    // mainScene->AddGameObject(player5);
    // mainScene->AddGameObject(player6);

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