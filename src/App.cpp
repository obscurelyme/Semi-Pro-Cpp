// clang-format off
#ifdef _DEBUG
#ifdef _WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif
// clang-format on

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <chrono>
#include <filesystem>
#include <iostream>

#include "Audio.hpp"
#include "Color.hpp"
#include "Cursor.hpp"
#include "Event.hpp"
#include "FPS.hpp"
#include "FontManager.hpp"
#include "Game/Animations/SpriteAnimation.hpp"
#include "Game/Collider.hpp"
#include "Game/Events.hpp"
#include "Game/Scene.hpp"
#include "Game/Scenes/MainScene.hpp"
#include "Game/Scenes/TestAnimations.hpp"
#include "Game/Scenes/TestBedScene.hpp"
#include "Game/Scenes/TitleScene.hpp"
#include "InputManager.hpp"
#include "Logger.hpp"
#include "Math.hpp"
#include "Renderer.hpp"
#include "Timer.hpp"
#include "Utilities.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/Text.hpp"
#include "Window.hpp"

bool paused = false;
bool quit = false;
SDL_Event event;

// SDL_AssertState appHandler(const SDL_AssertData* data, void*) {
//   std::cout << "Error executing function: " << data->function << std::endl;
//   return SDL_ASSERTION_IGNORE;
// };

int main(int, char**) {
  CoffeeMaker::Math::RandomEngine::Init();
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

  // SDL_SetAssertionHandler(appHandler, nullptr);

  CoffeeMaker::Utilities::Init(SDL_GetBasePath());
  CoffeeMaker::Audio::Init();
  CoffeeMaker::Texture::SetTextureDirectory();

  CoffeeMaker::BasicWindow win("Ultra Cosmo Invaders", 800, 600);
  CoffeeMaker::Renderer renderer;

  CoffeeMaker::Cursor cursor("cursor.png");
  CoffeeMaker::FontManager::Init();
  CoffeeMaker::FontManager::LoadFont("Sarpanch/Sarpanch-Regular");
  CoffeeMaker::FontManager::LoadFont("Sarpanch/Sarpanch-Bold");

  CoffeeMaker::Timer globalTimer;
  CoffeeMaker::FPS fpsCounter;

  CM_LOGGER_INFO("Display count: {}", win.DisplayCount());
  CM_LOGGER_INFO("Current Window DPI {}", win.GetScreenDPI().toString());

  SceneManager::AddScene(new TitleScene());
  SceneManager::AddScene(new MainScene());
  SceneManager::AddScene(new TestBedScene());
  SceneManager::AddScene(new TestAnimations());
  SceneManager::LoadScene();
  win.ShowWindow();
  CoffeeMaker::InputManager::Init();

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<float> elapsedSeconds = end - start;
  CM_LOGGER_INFO("Initialization time took: {}", elapsedSeconds.count());

  while (!quit) {
    // get input
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }

      if (event.type == SDL_USEREVENT) {
        CoffeeMaker::UserEventHandler::HandleUserEvent(event.user);
        if (event.user.code == CoffeeMaker::ApplicationEvents::COFFEEMAKER_GAME_PAUSE) {
          paused = true;
          SceneManager::PauseScene();
          CoffeeMaker::Timeout::PauseAllTimeouts();
        }
        if (event.user.code == CoffeeMaker::ApplicationEvents::COFFEEMAKER_GAME_UNPAUSE ||
            event.user.code == CoffeeMaker::ApplicationEvents::COFFEEMAKER_SCENE_LOAD) {
          paused = false;
          SceneManager::UnpauseScene();
          CoffeeMaker::Timeout::UnpauseAllTimeouts();
        }
      }

      CoffeeMaker::Button::PollEvents(&event);

      if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        CoffeeMaker::InputManager::HandleKeyBoardEvent(&event.key);
      }
    }

    CoffeeMaker::Timeout::ProcessTimeouts();
    Animations::SpriteAnimation::ProcessSpriteAnimations();

    float timeStep = globalTimer.GetTicks() / 1000.0f;

    // physics step
    Collider::PhysicsUpdate();

    // run logic
    fpsCounter.Update();
    SceneManager::UpdateCurrentScene(!paused ? timeStep : 0.0f);

    // Reset Timer
    globalTimer.Start();

    // render
    renderer.BeginRender();

    SceneManager::RenderCurrentScene();
    fpsCounter.Render();

    renderer.EndRender();
    // NOTE: uncomment here to view draw calls
    // CM_LOGGER_INFO("[Renderer][Draw Calls]: {}", CoffeeMaker::Renderer::DrawCalls());

    CoffeeMaker::InputManager::ClearAllPresses();
    CoffeeMaker::Button::ProcessEvents();
    Collider::ProcessCollisions();
  }

  CoffeeMaker::Audio::StopMusic();
  CoffeeMaker::Audio::Quit();
  SceneManager::DestroyAllScenes();
  CoffeeMaker::FontManager::Destroy();
  renderer.Destroy();
  SDL_Quit();

  CM_LOGGER_DESTROY();

#ifdef _DEBUG
#ifdef _WINDOWS
  _CrtDumpMemoryLeaks();
#endif
#endif

  return 0;
}
