#include "monsters.h"

#include <cmath>
#include <iostream> // for debug only
#include <utility>

namespace alatar {

const float kSpeedFactor = 1.0f;

/*void UpdateSlidingGate(MonsterInfo& info) {
  const int maxYExcursion = 20;
  const float delta_factor = 0.34f;  // TODO: move to an INI file?

  if (info.y_float >= static_cast<float>(info.y_initial)) {
    info.y_float = static_cast<float>(info.y_initial);
    info.y_delta = -delta_factor * kSpeedFactor;
  }

  if (info.y_float <= static_cast<float>(info.y_initial - maxYExcursion)) {
    info.y_float = static_cast<float>(info.y_initial - maxYExcursion);
    info.y_delta = delta_factor * kSpeedFactor;
  }

  info.y_float = info.y_float + info.y_delta;

  info.y = static_cast<int>(std::round(info.y_float));
}*/

void UpdateMonsters(MonsterClassArray& monster_info) {
  for (unsigned int ii = 0; ii < 6; ++ii) {
    if (monster_info[ii]->IsActive()) {
      monster_info[ii]->Update();
      /*switch (monster_info[ii].id) {
        case kNone:
          break;
        case kSlidingGate:
          UpdateSlidingGate(monster_info[ii]);
          break;
        default:
          break;
      }*/
    }
  }
}

//--------------------------------------------------------------------
// MonsterClass implementation
//--------------------------------------------------------------------
MonsterClass::MonsterClass(int id, int x, int y, int color, int sprite_id) {
  // TODO: Add validation here
  // TODO: Can I have a delegated constructor for the subclasses?
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
  y_delta_ = -1.0f * kDeltaFactor;
}

void SlidingGateClass::Update(void) {
  if (y_float_ >= static_cast<float>(y_initial_)) {
    y_float_ = static_cast<float>(y_initial_);
    y_delta_ = -kDeltaFactor * kSpeedFactor;
  }

  if (y_float_ <= static_cast<float>(y_initial_ - kMaxYExcursion)) {
    y_float_ = static_cast<float>(y_initial_ - kMaxYExcursion);
    y_delta_ = kDeltaFactor * kSpeedFactor;
  }

  y_float_ = y_float_ + y_delta_;

  y_ = static_cast<int>(std::round(y_float_));
}

}  // namespace alatar
