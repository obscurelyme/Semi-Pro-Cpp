#include "Game/Scene.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/Text.hpp"
#include "Widgets/UIComponent.hpp"
#include "Widgets/View.hpp"

class TitleScene : public Scene {
  public:
  virtual void Render();
  virtual void Update();
  virtual void Init();
  virtual void Destroy();

  void Play();
  void Quit();

  private:
  SDL_Color _backgroundColor;
  CoffeeMaker::Widgets::View* _view;
  CoffeeMaker::Widgets::Text* _title;
  CoffeeMaker::Button* _playButton;
  CoffeeMaker::Button* _quitButton;
  std::vector<CoffeeMaker::UIComponent*> _entities;
};
