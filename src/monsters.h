#ifndef H_ALATAR_MONSTERS
#define H_ALATAR_MONSTERS

#include <array>

namespace alatar {

enum MonsterType {
  kNone = 0,
  kArrow = 1,
  kBat = 2,
  kGhost = 3,
  kEvitWizard = 4,
  kWitch = 5,
  kFallingRock = 6,
  kElevator = 7,
  kLava = 8,
  kPit = 9,
  kTrapDoor = 10,
  kSlidingGate = 11,
  kLavaTroll = 12,
  kRollingRock = 13,
  kGiantRat = 14,
  kScorpion = 15,
  kSlime = 16,
  kGiantSpider = 17,
  kShadowLord = 18,
  kTheif = 19,
  kWizardsCat = 20
};

struct MonsterInfo {
  int id;
  bool active;
  int x_initial;
  int y_initial;
  int color;
  int sprite_id_initial;
  bool priority = true;
  // Stuff below this point is dynamic
  int x;
  int y;
  int sprite_id;
  float x_delta;
  float y_delta;
  float x_float;
  float y_float;
};

using MonsterInfoArray = std::array<MonsterInfo, 6>;

void UpdateMonsters(MonsterInfoArray& monster_info);

}  // namespace alatar

#endif
