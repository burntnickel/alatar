#include "monsters.h"

#include <cmath>
#include <iostream>  // for debug only
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
MonsterClass::MonsterClass(int id, int x, int y, int color, int sprite_id) {
  // TODO: Add validation here
  active_ = (id != kNone);
  id_ = id;
  x_initial_ = x;
  y_initial_ = y;
  color_ = color;
  sprite_id_initial_ = sprite_id;
  priority_ = true;

  x_ = x;
  y_ = y;
  x_float_ = static_cast<float>(x);
  y_float_ = static_cast<float>(y);
  sprite_id_ = sprite_id;
};

bool MonsterClass::IsActive(void) const {
  return active_;
}

int MonsterClass::GetId(void) const {
  return id_;
}

int MonsterClass::GetXInitial(void) const {
  return x_initial_;
}

int MonsterClass::GetYInitial(void) const {
  return y_initial_;
}

int MonsterClass::GetColor(void) const {
  return color_;
}

int MonsterClass::GetSpriteIDInitial(void) const {
  return sprite_id_initial_;
}

bool MonsterClass::GetPriority(void) const {
  return priority_;
}

MonsterClassPtr MonsterClass::MonsterClassFactory(int id, int x, int y, int color, int sprite_id) {
  switch (id) {
    case kSlidingGate:
      return std::make_unique<SlidingGateClass>(x, y, color, sprite_id);
      break;
    default:
      return std::make_unique<MonsterClass>(id, x, y, color, sprite_id);
  }
}

//--------------------------------------------------------------------
// SlidingGate implementation
//--------------------------------------------------------------------
SlidingGateClass::SlidingGateClass(int x, int y, int color, int sprite_id)
    : MonsterClass(kSlidingGate, x, y, color, sprite_id) {
  priority_ = false;
  // y_delta_ = -1.0f * kDeltaFactor;
  y_delta_ = -1.0f;
}

void SlidingGateClass::Update(Uint64 counter) {
  float elapsed_ms =
      static_cast<float>(counter - old_counter_) * static_cast<float>(alatar::gCounterToMsScale);

  y_float_ = y_float_ + y_delta_ * elapsed_ms * kPixelsPerMs;

  if (y_float_ >= static_cast<float>(y_initial_)) {
    y_float_ = static_cast<float>(y_initial_);
    y_delta_ = -1.0f;
  }

  if (y_float_ <= static_cast<float>(y_initial_ - kMaxYExcursion)) {
    y_float_ = static_cast<float>(y_initial_ - kMaxYExcursion);
    y_delta_ = 1.0f;
  }

  y_ = static_cast<int>(std::round(y_float_));

  /*if (y_float_ >= static_cast<float>(y_initial_)) {
    y_float_ = static_cast<float>(y_initial_);
    y_delta_ = -kDeltaFactor * kSpeedFactor;
  }

  if (y_float_ <= static_cast<float>(y_initial_ - kMaxYExcursion)) {
    y_float_ = static_cast<float>(y_initial_ - kMaxYExcursion);
    y_delta_ = kDeltaFactor * kSpeedFactor;
  }

  y_float_ = y_float_ + y_delta_;

  y_ = static_cast<int>(std::round(y_float_));*/

  MonsterClass::Update(counter);
}

}  // namespace alatar
