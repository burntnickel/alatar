#ifndef H_ALATAR_MONSTERS
#define H_ALATAR_MONSTERS

#include <SDL.h>

#include <array>
#include <memory>

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

class MonsterClass;  // Forward declaration to support the following type definition
using MonsterClassPtr = std::unique_ptr<MonsterClass>;

class MonsterClass {
 public:
  MonsterClass(void) = default;
  MonsterClass(int id, int x, int y, int color, int sprite_id);
  virtual ~MonsterClass(void) {};

 public:
  bool IsActive(void) const;
  int GetId(void) const;
  int GetXInitial(void) const;
  int GetYInitial(void) const;
  int GetColor(void) const;
  int GetSpriteIDInitial(void) const;
  bool GetPriority(void) const;

 public:
  // Defualt just updates the counter
  virtual void Update(Uint64 counter) { old_counter_ = counter; };

 public:
  // Stuff below this point is dynamic
  int x_;
  int y_;
  float x_float_;
  float y_float_;
  int sprite_id_;

 protected:
  bool active_ = false;
  int id_;
  int x_initial_;
  int y_initial_;
  int color_;
  int sprite_id_initial_;
  bool priority_;
  Uint64 old_counter_;

 public:
  static MonsterClassPtr MonsterClassFactory(int id, int x, int y, int color, int sprite_id);
};

class SlidingGateClass : public MonsterClass {
 public:
  SlidingGateClass(void) = delete;
  SlidingGateClass(int x, int y, int color, int sprite_id);

 public:
  void Update(Uint64 counter) override;

 public:
  float y_delta_;

 private:
  static constexpr int kMaxYExcursion = 20;
  static constexpr float kPixelsPerMs = 0.34f / (1000.0f / 60.0f);
  // static constexpr float kDeltaFactor = 0.34f;  // TODO: move to an INI file?
};

using MonsterClassArray = std::array<MonsterClassPtr, kMaxMonsters>;

void UpdateMonsters(MonsterClassArray& monster_info, Uint64 counter);

}  // namespace alatar

#endif
