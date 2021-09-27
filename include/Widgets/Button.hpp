#ifndef _coffeemaker_button_hpp
#define _coffeemaker_button_hpp

#include <SDL2/SDL.h>

#include <string>

#include "Color.hpp"
#include "Texture.hpp"
#include "Widgets/UIComponent.hpp"

namespace CoffeeMaker {

  class Button : public UIComponent {
    public:
    Button();

    void Draw();

    void OnEvent(const SDL_Event *e);
    void OnMouseover();
    void OnClick(void (*callback)(void));
    void OnMouseleave();

    void Render();

    int top;
    int left;
    int width;
    int height;
    int padding;
    void *children;
    Texture _texture;

    private:
    bool _HitDetection(const int &mouseX, const int &mouseY);

    bool _hovered;
  };

}  // namespace CoffeeMaker

#endif
