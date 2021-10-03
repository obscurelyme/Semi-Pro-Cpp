#ifndef _game_entity_hpp
#define _game_entity_hpp

enum class EnemyAnimationState {
  Idle,
  Moving
};

class Entity {
  public:
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;

    ~Entity() = default;
};

#endif