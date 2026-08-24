#ifndef H_ALATAR_MONSTERS
#define H_ALATAR_MONSTERS

#include <SDL.h>

#include <array>
#include <memory>

#include "sprites_classic.h"

namespace alatar {

const int kMaxMonsters = 6;

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

struct MonsterData {
  unsigned int id;
  unsigned int x;
  unsigned int y;
  unsigned int color;
  unsigned int sprite_id;
  unsigned int animation_length;
  int elevator_dx;
  int elevator_dy;
  unsigned int elevator_duration;
};

//----------------------------------------------
// MonsterClass
//----------------------------------------------
class MonsterClass;  // Forward declaration to support the following type definition
using MonsterClassPtr = std::unique_ptr<MonsterClass>;
using MonsterClassArray = std::array<MonsterClassPtr, kMaxMonsters>;

class MonsterClass {
 public:
  MonsterClass(void) = default;
  explicit MonsterClass(MonsterData monster_data);
  virtual ~MonsterClass(void) {};

 public:
  bool IsActive(void) const;
  unsigned int GetId(void) const;
  unsigned int GetXInitial(void) const;
  unsigned int GetYInitial(void) const;
  unsigned int GetColor(void) const;
  unsigned int GetSpriteIDInitial(void) const;
  unsigned int GetAnimationLength(void) const;
  bool GetPriority(void) const;
  int GetSpriteMods(void) const;

 public:
  // Defualt just updates the counter
  virtual void Update(Uint64 counter);

  //friend void UpdateMonsters(MonsterClassArray& monster_info, Uint64 counter);

 public:
  // Stuff below this point is dynamic
  unsigned int x_;
  unsigned int y_;
  float x_float_;
  float y_float_;
  unsigned int sprite_id_;

 protected:
  bool active_ = false;
  unsigned int id_;
  unsigned int x_initial_;
  unsigned int y_initial_;
  unsigned int color_;
  unsigned int sprite_id_initial_;
  unsigned int animation_length_;
  bool priority_;
  int sprite_mods_ = alatar_classic::kSpriteMultiColor;
  Uint64 old_counter_;
  bool old_counter_valid_ = false;
  unsigned int animation_state_ = 0;
  float animation_counter_ = 0.0f;
  float elapsed_ms_ = 0.0;
  float frames_per_ms_ = 0.0; 
  

 public:
  static MonsterClassPtr MonsterClassFactory(MonsterData monster_data);
};

//----------------------------------------------
// ElevatorClass
//----------------------------------------------
class ElevatorClass : public MonsterClass {
 public:
  ElevatorClass(void) = delete;
  explicit ElevatorClass(MonsterData monster_data);

 public:
  void Update(Uint64 counter) override;

 public:
  float duration_;
  float ticks_ = 0.0f;
  bool tick_up_ = true;
  float x_delta_;
  float y_delta_;

  // Should some of these be moved to an INI file?
 private:
  static constexpr float kTicksPerMs = 0.021f;
};

//----------------------------------------------
// LavaClass
//----------------------------------------------
class LavaClass : public MonsterClass {
 public:
  LavaClass(void) = delete;
  explicit LavaClass(MonsterData monster_data);

 public:
  void Update(Uint64 counter) override;
};

//----------------------------------------------
// SlidingGateClass
//----------------------------------------------
class SlidingGateClass : public MonsterClass {
 public:
  SlidingGateClass(void) = delete;
  explicit SlidingGateClass(MonsterData monster_data);

 public:
  void Update(Uint64 counter) override;

 public:
  float y_delta_;

  // Should some of these be moved to an INI file?
 private:
  static constexpr int kMaxYExcursion = 20;
  static constexpr float kPixelsPerMs = 0.36f / (1000.0f / 60.0f);
};

// Misc. stuff
void UpdateMonsters(MonsterClassArray& monster_info, Uint64 counter);

}  // namespace alatar

#endif
