#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <chrono>
#include <filesystem>

#include "Color.hpp"
#include "Cursor.hpp"
#include "FPS.hpp"
#include "FontManager.hpp"
#include "Game/Collider.hpp"
#include "Game/Enemy.hpp"
#include "Game/Player.hpp"
#include "Game/Tiles.hpp"
#include "InputManager.hpp"
#include "Logger.hpp"
#include "Renderer.hpp"
#include "TextView.hpp"
#include "Timer.hpp"
#include "Utilities.hpp"
#include "Window.hpp"

bool quit = false;
SDL_Event event;

int main(int, char **) {
  // Start clock
  auto start = std::chrono::steady_clock::now();
  CM_LOGGER_INIT();

  if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
    CM_LOGGER_CRITICAL("Could not initialize SDL2!");
    exit(1);
  }

  if (IMG_Init(IMG_INIT_PNG) == 0) {
    CM_LOGGER_CRITICAL("Could not initialize SDL2 Images");
    exit(1);
  }

  CoffeeMaker::Utilities::Init(SDL_GetBasePath());
  CoffeeMaker::Texture::SetTextureDirectory();

  CoffeeMaker::BasicWindow win("Hello, SDL!", 800, 600);
  CoffeeMaker::Renderer renderer;

  // CoffeeMaker::Cursor cursor("cursor.png");
  CoffeeMaker::FontManager::Init();
  CoffeeMaker::FontManager::LoadFont("Roboto/Roboto-Regular");
  CoffeeMaker::FontManager::LoadFont("Roboto/Roboto-Black");

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<float> elapsedSeconds = end - start;

  CoffeeMaker::Timer timer;
  CoffeeMaker::FPS fpsCounter;

  CM_LOGGER_INFO("Initialization time took: {}", elapsedSeconds.count());
  CM_LOGGER_INFO("Display count: {}", win.DisplayCount());
  CM_LOGGER_INFO("Current Window DPI {}", win.GetScreenDPI().toString());

  Enemy enemy;
  Player player;
  Collider collide(true);
  collide.SetHeight(64);
  collide.SetWidth(32);

  Tiles tiles("space.png", 800, 600);

  win.ShowWindow();
  CoffeeMaker::InputManager::Init();

  while (!quit) {
    // get input
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }

      if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        CoffeeMaker::InputManager::HandleKeyBoardEvent(&event.key);
      }
    }

    // physics step
    Collider::PhysicsUpdate();

    // run logic
    player.Update();
    enemy.Update();
    fpsCounter.Update();

    // render
    renderer.BeginRender();

    tiles.Render();
    player.Render();
    enemy.Render();
    fpsCounter.Render();
    collide.Render();

    renderer.EndRender();

    CoffeeMaker::InputManager::ClearAllPresses();
    // Cap framerate
    SDL_Delay(16);
  }

  renderer.Destroy();
  SDL_Quit();
  CM_LOGGER_DESTROY();

  return 0;
}
