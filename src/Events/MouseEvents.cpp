#include "Events/MouseEvents.hpp"

unsigned int CoffeeMaker::IMouseListener::_uid = 0;
std::vector<CoffeeMaker::IMouseListener*> CoffeeMaker::IMouseListener::_listeners = {};

void CoffeeMaker::MouseEventHandler::HandleMouseEvents(const SDL_Event& event) {
  if (event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEBUTTONDOWN) {
    CoffeeMaker::IMouseListener::ProcessMouseButtonEvent(event.button);
  }

  if (event.type == SDL_MOUSEMOTION) {
    CoffeeMaker::IMouseListener::ProcessMouseMotionEvent(event.motion);
  }

  if (event.type == SDL_MOUSEWHEEL) {
    CoffeeMaker::IMouseListener::ProcessMouseWheelEvent(event.wheel);
  }
}

CoffeeMaker::IMouseListener::IMouseListener() : _id(++_uid), _active(true) {
  _instances.push_back(this);
  _index = _listeners.size() - 1;
}

CoffeeMaker::IMouseListener::~IMouseListener() { _listeners[_index] = nullptr; }

void CoffeeMaker::IMouseListener::RemoveStaleListeners() {
  for (auto it = _listeners.begin(); it != _listeners.end();) {
    if ((*it) == nullptr) {
      it = _listeners.erase(it);
    } else {
      ++it;
    }
  }
}

void CoffeeMaker::IMouseListener::ProcessMouseWheelEvent(const SDL_MouseWheelEvent& event) {
  for (auto& listener : _listeners) {
    listener->OnMouseWheel(event);
  }
}

void CoffeeMaker::IMouseListener::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& event) {
  for (auto& listener : _listeners) {
    listener->OnMouseMove(event);
  }
}

void CoffeeMaker::IMouseListener::ProcessMouseButtonEvent(const SDL_MouseButtonEvent& event) {
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    for (auto& listener : _listeners) {
      listener->OnMouseDown(event);
    }
    return;
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    for (auto& listener : _listeners) {
      listener->OnMouseUp(event);
    }
  }
}
