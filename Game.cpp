#include "Game.hpp"
#include "Components.hpp"
#include "CustomClasses.hpp"
#include "Global.hpp"
#include "Helper.hpp"
#include "Physic2D.hpp"
#include "SDLCustomEvent.hpp"

#include <cmath>
#include <iostream>

SDL_Event Game::event;

Game::Game() {
    isRunning = false;
}

Game::~Game() {
}

void Game::init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen) {
    reset = false;
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

        // Initialize SDL_ttf
        if (TTF_Init() == -1) {
            std::cerr << "Failed to initialize TTF: " << TTF_GetError() << std::endl;
            isRunning = false;
            return;
        }

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
    mainScene->AssignLogic([mainScene]() {

#pragma region Background Setup
        GameObject *background = new GameObject("Background");
        background->transform.position = Vector2(640, 360);
        background->transform.scale = Vector2(1, 1);

        background->AddComponent(new SpriteRenderer(background, Vector2(1280, 720), -10, LoadSpriteSheet("Assets/Sprites/yard.png")));

        GameObjectManager::GetInstance()->AddGameObject(background);
#pragma endregion

#pragma region Ball Setup
        GameObject *ball = new GameObject("Ball");
        ball->tag = 3;
        ball->transform.position = Vector2(640, 360);
        ball->transform.scale = Vector2(2, 2);

        ball->AddComponent(new SpriteRenderer(ball, Vector2(15, 15), 10, LoadSpriteSheet("Assets/default.png")));

        ball->AddComponent(new Animator(ball, {AnimationClip("Roll", "Assets/soccer_ball.png", Vector2(15, 15), 1000, true, 1.0, 0, 1)}));
        ball->GetComponent<Animator>()->Play("Roll");

        ball->AddComponent(new Rigidbody2D(ball, 1, 0.025, .9));

        ball->AddComponent(new VelocityToAnimSpeedController(ball, "Roll"));
        ball->AddComponent(new StayInBounds(ball, false));

        ball->AddComponent(new CircleCollider2D(ball, Vector2(0, 0), 7.5));

        ball->AddComponent(new BallStateMachine(ball, 2.0, 50, 100));

        ball->GetComponent<CircleCollider2D>()->OnCollisionEnter.addHandler(
            [ball](Collider2D *collider) {
                ball->GetComponent<BallStateMachine>()->OnCollisionEnter(collider);
            });

        GameObjectManager::GetInstance()->AddGameObject(ball);
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


        player1->transform.position = Vector2(75, HEIGHT / 2); // Centered vertically
        GameObject *player2 = GameObject::Instantiate("Player2", player1, Vector2(150, HEIGHT / 2 + 60), 0, Vector2(2, 2));
        GameObject *player3 = GameObject::Instantiate("Player3", player1, Vector2(150, HEIGHT / 2 - 60), 0, Vector2(2, 2));

        GameObject *player4 = GameObject::Instantiate("Player4", player1, Vector2(WIDTH - 150, HEIGHT / 2 - 60), 0, Vector2(2, 2));
        GameObject *player5 = GameObject::Instantiate("Player5", player1, Vector2(WIDTH - 150, HEIGHT / 2), 0, Vector2(2, 2));
        GameObject *player6 = GameObject::Instantiate("Player6", player1, Vector2(WIDTH - 75, HEIGHT / 2 + 60), 0, Vector2(2, 2));

        player1->tag = player2->tag = player3->tag = 1;
        player4->tag = player5->tag = player6->tag = 2;

        player1->AddComponent(new Animator(player1, {AnimationClip("Run", "Assets/Sprites/football.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));
        player2->AddComponent(new Animator(player2, {AnimationClip("Run", "Assets/Sprites/football2.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));
        player3->AddComponent(new Animator(player3, {AnimationClip("Run", "Assets/Sprites/football3.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));
        player4->AddComponent(new Animator(player4, {AnimationClip("Run", "Assets/Sprites/football4.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));
        player5->AddComponent(new Animator(player5, {AnimationClip("Run", "Assets/Sprites/football5.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));
        player6->AddComponent(new Animator(player6, {AnimationClip("Run", "Assets/Sprites/football6.png", Vector2(32, 32), 1000, true, 1.0, 0, 5)}));

        auto setupCollisionHandler = [](GameObject *player) {
            player->GetComponent<CircleCollider2D>()->OnCollisionEnter.addHandler(
                [player](Collider2D *collider) {
                    if (collider->gameObject->tag == 4) {
                        Rigidbody2D *rigidbody = player->GetComponent<Rigidbody2D>();
                        rigidbody->BounceOff(collider->GetNormal(player->transform.position));
                    }
                });
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

        if (Player2Mode) {
            player4->AddComponent(new MovementController(player4, 10, false));
            player5->AddComponent(new MovementController(player5, 10, false));
            player6->AddComponent(new MovementController(player6, 10, false));
        }

        player1->AddComponent(new KickControl(player1, ball, SDLK_SPACE, HIGH_KICK_FORCE));
        player2->AddComponent(new KickControl(player2, ball, SDLK_SPACE, LOW_KICK_FORCE));
        player3->AddComponent(new KickControl(player3, ball, SDLK_SPACE, HIGH_KICK_FORCE));

        if (Player2Mode) {
            player4->AddComponent(new KickControl(player4, ball, SDLK_KP_ENTER, HIGH_KICK_FORCE));
            player5->AddComponent(new KickControl(player5, ball, SDLK_KP_ENTER, LOW_KICK_FORCE));
            player6->AddComponent(new KickControl(player6, ball, SDLK_KP_ENTER, HIGH_KICK_FORCE));
        }

        player1->AddComponent(new AIGoalKeeper(player1, ball, 10, true));
        player2->AddComponent(new AIDefender(player2, ball, 10, true));
        player3->AddComponent(new AIAttacker(player3, ball, 10, true));

        player4->AddComponent(new AIAttacker(player4, ball, 10, false));
        player5->AddComponent(new AIDefender(player5, ball, 10, false));
        player6->AddComponent(new AIGoalKeeper(player6, ball, 10, false));

        // First controller switcher for player1, player2, and player3
        GameObject *controllerSwitcher1 = new GameObject("ControllerSwitcher1");
        TeamControl *movementControllerSwitcher1 = dynamic_cast<TeamControl *>(controllerSwitcher1->AddComponent(
            new TeamControl(controllerSwitcher1, LoadSpriteSheet("Assets/blue_indicator.png"), 75.0)));
        movementControllerSwitcher1->AddMovementController(SDLK_1, player1->GetComponent<MovementController>());
        movementControllerSwitcher1->AddMovementController(SDLK_2, player2->GetComponent<MovementController>());
        movementControllerSwitcher1->AddMovementController(SDLK_3, player3->GetComponent<MovementController>());
        GameObjectManager::GetInstance()->AddGameObject(controllerSwitcher1);

        if (Player2Mode) {
            // Second controller switcher for player4, player5, and player6
            GameObject *controllerSwitcher2 = new GameObject("ControllerSwitcher2");
            TeamControl *movementControllerSwitcher2 = dynamic_cast<TeamControl *>(controllerSwitcher2->AddComponent(
                new TeamControl(controllerSwitcher2, LoadSpriteSheet("Assets/red_indicator.png"), 75.0)));
            movementControllerSwitcher2->AddMovementController(SDLK_KP_4, player4->GetComponent<MovementController>());
            movementControllerSwitcher2->AddMovementController(SDLK_KP_5, player5->GetComponent<MovementController>());
            movementControllerSwitcher2->AddMovementController(SDLK_KP_6, player6->GetComponent<MovementController>());
            GameObjectManager::GetInstance()->AddGameObject(controllerSwitcher2);
        }

        GameObjectManager::GetInstance()->AddGameObject(player1);
        GameObjectManager::GetInstance()->AddGameObject(player2);
        GameObjectManager::GetInstance()->AddGameObject(player3);
        GameObjectManager::GetInstance()->AddGameObject(player4);
        GameObjectManager::GetInstance()->AddGameObject(player5);
        GameObjectManager::GetInstance()->AddGameObject(player6);

#pragma endregion

#pragma region Goal Setup
        GameObject *goal1 = new GameObject("Goal1");
        goal1->transform.position = Vector2(30, 360);
        goal1->transform.scale = Vector2(5, 4);
        goal1->tag = 5;

        goal1->AddComponent(new SpriteRenderer(goal1, Vector2(16, 53), 0, LoadSpriteSheet("Assets/Sprites/goal.png")));
        goal1->AddComponent(new BoxCollider2D(goal1, Vector2(0, 0), 
            Vector2(16 * goal1->transform.scale.x, 53 * goal1->transform.scale.y)
        ));

        goal1->GetComponent<BoxCollider2D>()->OnCollisionEnter.addHandler(
            [goal1](Collider2D *collider) {
                BoxCollider2D *goal1Col = goal1->GetComponent<BoxCollider2D>();
                if (collider->gameObject->tag == 3) {
                    if (goal1Col->GetNormal(collider->gameObject->transform.position) == Vector2(1, 0)) {
                        std::cout << "Goal!!! Right team scored!" << std::endl;
                        SDL_UserEvent event;
                        event.type = SDL_GOAL1_EVENT_TYPE;
                        SDL_PushEvent((SDL_Event *)&event);
                    } else {
                        Rigidbody2D *rigidbody = collider->gameObject->GetComponent<Rigidbody2D>();
                        rigidbody->BounceOff(goal1Col->GetNormal(collider->gameObject->transform.position));
                    }
                } else {
                    Rigidbody2D *rigidbody = collider->gameObject->GetComponent<Rigidbody2D>();
                    collider->gameObject->transform.position += goal1Col->GetNormal(collider->gameObject->transform.position) * rigidbody->velocity.Magnitude();
                }
            });

        GameObjectManager::GetInstance()->AddGameObject(goal1);

        GameObject *goal2 = new GameObject("Goal2");
        goal2->transform.position = Vector2(1250, 360);
        goal2->transform.rotation = 180;
        goal2->transform.scale = Vector2(5, 4);
        goal2->tag = 6;

        goal2->AddComponent(new SpriteRenderer(goal2, Vector2(16, 53), 0, LoadSpriteSheet("Assets/Sprites/goal.png")));
        goal2->AddComponent(new BoxCollider2D(goal2, Vector2(0, 0), 
            Vector2(16 * goal2->transform.scale.x, 53 * goal2->transform.scale.y)
        ));

        goal2->GetComponent<BoxCollider2D>()->OnCollisionEnter.addHandler(
            [goal2](Collider2D *collider) {
                BoxCollider2D *goal2Col = goal2->GetComponent<BoxCollider2D>();
                if (collider->gameObject->tag == 3) {
                    if (goal2Col->GetNormal(collider->gameObject->transform.position) == Vector2(-1, 0)) {
                        std::cout << "Goal!!! Left team scored!" << std::endl;

                        SDL_UserEvent event;
                        event.type = SDL_GOAL2_EVENT_TYPE;
                        SDL_PushEvent((SDL_Event *)&event);

                        return;
                    } else {
                        Rigidbody2D *rigidbody = collider->gameObject->GetComponent<Rigidbody2D>();
                        rigidbody->BounceOff(goal2Col->GetNormal(collider->gameObject->transform.position));
                    }
                } else {
                    Rigidbody2D *rigidbody = collider->gameObject->GetComponent<Rigidbody2D>();
                    collider->gameObject->transform.position += goal2Col->GetNormal(collider->gameObject->transform.position) * rigidbody->velocity.Magnitude();
                }
            });

        GameObjectManager::GetInstance()->AddGameObject(goal2);

#pragma endregion
    });
    SceneManager::GetInstance()->AddScene(mainScene);
    SceneManager::GetInstance()->LoadScene("Main");
}

void Game::handleEvents() {

    SDL_PollEvent(&Game::event);

    if (event.type == SDL_QUIT) {
        isRunning = false;
        return;
    }

    if (event.type == SDL_GOAL1_EVENT_TYPE) {
        std::cout << "Goal!!! Right team scored!" << std::endl;
        SceneManager::GetInstance()->LoadScene("Main");
        return;
    }

    if (event.type == SDL_GOAL2_EVENT_TYPE) {
        std::cout << "Goal!!! Left team scored!" << std::endl;
        SceneManager::GetInstance()->LoadScene("Main");
        return;
    }
}

void Game::update() {
    SceneManager::GetInstance()->Update();
}

void Game::render() {
    SDL_RenderClear(renderer);

    // Show score
    SDL_Color textColor = {0, 255, 0, 255};
    std::string scoreText = std::to_string(scoreTeam1) + " - " + std::to_string(scoreTeam2);
    SDL_Texture* scoreTexture = LoadFontTexture(scoreText, "Assets/Fonts/arial.ttf", textColor, 24);
    if (scoreTexture) {
        RenderTexture(scoreTexture, 640, 20);
        SDL_DestroyTexture(scoreTexture);
    } else {
        std::cerr << "Failed to load score texture" << std::endl;
    }

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
    // Quit SDL_ttf
    TTF_Quit();
    SDL_Quit();
    std::cout << "Game cleaned..." << std::endl;
}

bool Game::running() {
    return isRunning;
}

bool Game::reseting() {
    return reset;
}