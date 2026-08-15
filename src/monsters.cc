#include "monsters.h"

#include <cmath>

namespace alatar {

const float kSpeedFactor = 1.0f;

void UpdateSlidingGate(MonsterInfo& info) {
  const int maxYExcursion = 20;
  const float delta_factor = 0.34f; // TODO: move to an INI file?

  if (info.y_float >= static_cast<float>(info.y_initial)) {
    info.y_float = info.y_initial;
    info.y_delta = -delta_factor * kSpeedFactor;
  }

  if (info.y_float <= (info.y_initial - maxYExcursion)) {
    info.y_float = info.y_initial - maxYExcursion;
    info.y_delta = delta_factor * kSpeedFactor;
  }

  info.y_float = info.y_float + info.y_delta;

  info.y = std::round(info.y_float);
}

void UpdateMonsters(MonsterInfoArray& monster_info) {
  for (unsigned int ii = 0; ii < 6; ++ii) {
    if (monster_info[ii].active) {
      switch (monster_info[ii].id) {
        case kNone:
          break;
        case kSlidingGate:
          UpdateSlidingGate(monster_info[ii]);
          break;
        default:
          break;
      }
    }
  }
}

}  // namespace alatar
