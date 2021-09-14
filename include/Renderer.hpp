#ifndef _coffeemaker_renderer_hpp
#define _coffeemaker_renderer_hpp

#include <SDL2/SDL.h>
#include "Window.hpp"

namespace CoffeeMaker
{
  class IRenderer
  {
    public:
      virtual ~IRenderer() = default;
      virtual SDL_Renderer * Instance() = 0;
  };

  class Renderer
  {
  public:
    Renderer();

    static SDL_Renderer * Instance();
    void Render();
    void BeginRender();
    void EndRender();
    void Destroy();

  private:
    static SDL_Renderer *_renderer;
  };

} // namespace CoffeeMaker

#endif