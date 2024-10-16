#include "Game.hpp"
#include "CustomClasses.hpp"
#include "Global.hpp"
#include <iostream>

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

void dummy() {
    std::cout << "Dummy" << std::endl;
}

GameObject *player = new GameObject("Player");

void Game::objectInit() {
    // Add your object initialisation here

    std::cout << "Object Initialisation..." << std::endl;

    // Test scene
    Scene *main = new Scene("Main");

    player->transform.position = Vector2(150, 200);
    player->transform.scale = Vector2(5, 5);
    player->AddComponent(new SpriteRenderer(player, renderer, Vector2(35, 44)));

    SpriteRenderer *spriteRenderer = player->GetComponent<SpriteRenderer>();
    spriteRenderer->spriteSheet = LoadSpriteSheet("Assets/default.png");

    AnimationClip playerIdle = AnimationClip("Idle", "Assets/kirby_fall.png", Vector2(35, 44), 2000, true, 1.0, 0, 14);
    // playerIdle.onComplete->addHandler(dummy);

    player->AddComponent(new Animator(player, {playerIdle}));
    player->GetComponent<Animator>()->Play("Idle");

    SceneManager::GetInstance()->AddScene(main);
    SceneManager::GetInstance()->LoadScene("Main");
}

void Game::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
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

GameObject* previousObject = player;

void Game::update() {
    // Game logic updates go here

    // TEST INSTANTIATE
    if (SDL_GetTicks() - lastSpawnTime >= spawnCooldown) {
        GameObject *newObject = GameObject::Instantiate("Player" + std::to_string(spawnCount), 
        *player, previousObject->transform.position + Vector2(10, 0), previousObject->transform.rotation+45, Vector2(5, 5));
        lastSpawnTime = SDL_GetTicks();
        previousObject = newObject;
        spawnCount++;
    }
    //

    SceneManager::GetInstance()->Update();
}

void Game::render() {
    SDL_RenderClear(renderer);
    // Add your rendering stuff here

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