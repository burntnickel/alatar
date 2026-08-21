#include "monsters.h"

#include <cmath>
#include <iostream>  // for debug
#include <utility>

#include "globals.h"

namespace alatar {

const float kSpeedFactor = 1.0f;

void UpdateMonsters(MonsterClassArray& monster_info, Uint64 counter) {
  for (unsigned int ii = 0; ii < 6; ++ii) {
    if (monster_info[ii]->IsActive()) {
      monster_info[ii]->Update(counter);
    }
  }
}

//--------------------------------------------------------------------
// MonsterClass implementation
//--------------------------------------------------------------------
MonsterClass::MonsterClass(MonsterData monster_data) {
  // TODO: Add validation here
  active_ = (monster_data.id != kNone);
  id_ = monster_data.id;
  x_initial_ = monster_data.x;
  y_initial_ = monster_data.y;
  color_ = monster_data.color;
  sprite_id_initial_ = monster_data.sprite_id;
  animation_length_ = monster_data.animation_length;
  priority_ = true;

  x_ = monster_data.x;
  y_ = monster_data.y;
  x_float_ = static_cast<float>(monster_data.x);
  y_float_ = static_cast<float>(monster_data.y);
  sprite_id_ = monster_data.sprite_id;
};

bool MonsterClass::IsActive(void) const {
  return active_;
}

unsigned int MonsterClass::GetId(void) const {
  return id_;
}

unsigned int MonsterClass::GetXInitial(void) const {
  return x_initial_;
}

unsigned int MonsterClass::GetYInitial(void) const {
  return y_initial_;
}

unsigned int MonsterClass::GetColor(void) const {
  return color_;
}

unsigned int MonsterClass::GetSpriteIDInitial(void) const {
  return sprite_id_initial_;
}

unsigned int MonsterClass::GetAnimationLength(void) const {
  return animation_length_;
}

bool MonsterClass::GetPriority(void) const {
  return priority_;
}

int MonsterClass::GetSpriteMods(void) const {
  return sprite_mods_;
}

MonsterClassPtr MonsterClass::MonsterClassFactory(MonsterData monster_data) {
  switch (monster_data.id) {
    case kSlidingGate:
      return std::make_unique<SlidingGateClass>(monster_data);
      break;
    default:
      return std::make_unique<MonsterClass>(monster_data);
  }
}

//--------------------------------------------------------------------
// SlidingGate implementation
//--------------------------------------------------------------------
SlidingGateClass::SlidingGateClass(MonsterData monster_data) : MonsterClass(monster_data) {
  priority_ = false;
  y_delta_ = -1.0f;
  sprite_mods_ = alatar_classic::kSpriteMultiColor;
}

void SlidingGateClass::Update(Uint64 counter) {
  float elapsed_ms =
      static_cast<float>(counter - old_counter_) * static_cast<float>(alatar::gCounterToMsScale);

  // Adjust y location
  y_float_ = y_float_ + y_delta_ * elapsed_ms * kPixelsPerMs * kSpeedFactor;

  if (y_float_ >= static_cast<float>(y_initial_)) {
    y_float_ = static_cast<float>(y_initial_);
    y_delta_ = -1.0f;
  }

  if (y_float_ <= static_cast<float>(y_initial_ - kMaxYExcursion)) {
    y_float_ = static_cast<float>(y_initial_ - kMaxYExcursion);
    y_delta_ = 1.0f;
  }

  y_ = static_cast<unsigned int>(std::round(y_float_));

  // Adjust animation sprite ID
  animation_counter_ = animation_counter_ + kFramesPerMs * elapsed_ms;
  animation_state_ = static_cast<unsigned int>(animation_counter_);

  // std::cout << animation_length_ << " : " << animation_state_ << " : " << animation_counter_ << "\n";

  if (animation_state_ >= animation_length_) {
    animation_counter_ = animation_counter_ - static_cast<float>(animation_length_);
    animation_state_ = 0;

    // If we're really running behind the animation counter will be greater than 1 at this point so we need to
    // check and correct
    if (animation_counter_ >= 1.0f) {
      animation_counter_ = 0.0f;
    }
  }

  sprite_id_ = sprite_id_initial_ + animation_state_;

  MonsterClass::Update(counter);
}

}  // namespace alatar
