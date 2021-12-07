// clang-format off
#ifdef _DEBUG_D
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

#include <argparse/argparse.hpp>
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
#include "Game/Scenes/All.hpp"
#include "Game/ScoreManager.hpp"
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

int main(int argc, char** argv) {
  argparse::ArgumentParser program("ultra-cosmo-invaders", "0.1.0");
  program.add_argument("-s", "--scene").default_value(0).help("load a specific scene").scan<'i', int>().nargs(1);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  CoffeeMaker::Math::RandomEngine::Init();
  // Start clock
  auto start = std::chrono::steady_clock::now();
  CM_LOGGER_INIT();

  if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
    CM_LOGGER_CRITICAL("Could not initialize SDL2!");
    CM_LOGGER_CRITICAL("Reason: {}", SDL_GetError());
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

  std::string basePath = CoffeeMaker::Utilities::BaseDirectory();
  basePath += "/assets/images/Player-NoBkGrd.png";
  SDL_Surface* iconSurface = IMG_Load(basePath.c_str());
  SDL_SetWindowIcon(win.Handle(), iconSurface);
  SDL_FreeSurface(iconSurface);
  CoffeeMaker::Cursor cursor("cursor.png");
  CoffeeMaker::FontManager::Init();
  CoffeeMaker::FontManager::LoadFont("Sarpanch/Sarpanch-Regular");
  CoffeeMaker::FontManager::LoadFont("Sarpanch/Sarpanch-Bold");
  CoffeeMaker::Timer globalTimer;
  CoffeeMaker::FPS fpsCounter;

  CM_LOGGER_INFO("Display count: {}", win.DisplayCount());
  CM_LOGGER_INFO("Current Window DPI {}", win.GetScreenDPI().toString());

  ScoreManager::Init();
  SceneManager::AddScene(new TitleScene());
  SceneManager::AddScene(new MainScene());
  SceneManager::AddScene(new HighScoreScene());
  // SceneManager::AddScene(new TestBedScene());
  // SceneManager::AddScene(new TestAnimations());
  // SceneManager::AddScene(new TestEchelonScene());
  // SceneManager::AddScene(new SplineBuilder());

  CoffeeMaker::Logger::Debug("Loading scene at index...{}", program.get<int>("--scene"));
  if (!SceneManager::LoadScene(program.get<int>("--scene"))) {
    quit = true;
  } else {
    win.ShowWindow();
    CoffeeMaker::InputManager::Init();

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedSeconds = end - start;
    CM_LOGGER_INFO("Initialization time took: {}", elapsedSeconds.count());

    CoffeeMaker::UserEventHandler::RegisterUserEvents(UCI::NumEventsToRegister());
  }

  while (!quit) {
    // get input
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
        SceneManager::DestroyCurrentScene();
        break;
      }

      if (event.type >= SDL_USEREVENT && event.type < SDL_LASTEVENT) {
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
      CoffeeMaker::MouseEventHandler::HandleMouseEvents(event);
      if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        CoffeeMaker::InputManager::HandleKeyBoardEvent(&event.key);
      }
    }

    if (!quit) {
      CoffeeMaker::Timeout::ProcessTimeouts();
      Animations::SpriteAnimation::ProcessSpriteAnimations();

      float timeStep = globalTimer.GetTicks() / 1000.0f;

      // physics step
      Collider::PhysicsUpdate();

      // run logic
      // fpsCounter.Update();
      SceneManager::UpdateCurrentScene(!paused ? timeStep : 0.0f);

      // Reset Timer
      globalTimer.Start();

      // render
      renderer.BeginRender();

      SceneManager::RenderCurrentScene();
      // fpsCounter.Render();

      renderer.EndRender();
      // NOTE: uncomment here to view draw calls
      // CM_LOGGER_INFO("[Renderer][Draw Calls]: {}", CoffeeMaker::Renderer::DrawCalls());

      CoffeeMaker::InputManager::ClearAllPresses();
      CoffeeMaker::Button::ProcessEvents();
      Collider::ProcessCollisions();
    }
  }

  ScoreManager::Destroy();
  CoffeeMaker::Audio::StopMusic();
  CoffeeMaker::Audio::Quit();
  SceneManager::DestroyAllScenes();
  CoffeeMaker::FontManager::Destroy();
  renderer.Destroy();
  SDL_Quit();

  CM_LOGGER_DESTROY();

#ifdef _DEBUG_D
#ifdef _WINDOWS
  _CrtDumpMemoryLeaks();
#endif
#endif

  return 0;
}
