#include "wiz_level.h"

#include <algorithm>
#include <array>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

#include "util.h"

namespace alatar {

std::array<const std::string, 16> kColorStrings = {{"black", "white", "red", "cyan", "purple", "green",
                                                    "blue", "yellow", "orange", "brown", "pink", "dark grey",
                                                    "grey", "light green", "light blue", "light grey"}};

std::array<const std::string, 12> kSpellNames = {{"Fire Ball", "Magic Missile", "Disintegrate", "Enchantment",
                                                  "Freeze", "Invisibility", "Teleport", "Feather Fall",
                                                  "Levitate", "Haste", "Slow", "none"}};

std::array<const std::string, 21> kMonsterNames = {
    {"none",      "Arrow",    "Bat",   "Ghost",        "Evil Wizard",  "Witch",      "Falling Rock",
     "Elevator",  "Lava",     "Pit",   "Trap Door",    "Sliding Gate", "Lava Troll", "Rolling Rock",
     "Giant Rat", "Scorpion", "Slime", "Giant Spider", "Shadow Lord",  "Thief",      "Wizard's Cat"}};

static unsigned char ColorRangeCheck(unsigned char c) {
  if (c < kColorStrings.size()) {
    return c;
  }

  throw std::out_of_range("Color code out of range");
}

std::string GetC64Color(unsigned char c) {
  return kColorStrings[ColorRangeCheck(c)];
}

static unsigned char SpellRangeCheck(unsigned char c) {
  if (c < kSpellNames.size()) {
    return c;
  }

  throw std::out_of_range("Spell number out of range");
}

std::string GetSpellName(unsigned char c) {
  return kSpellNames[SpellRangeCheck(c)];
}

static unsigned char MonsterRangeCheck(unsigned char c) {
  if (c < kMonsterNames.size()) {
    return c;
  }

  throw std::out_of_range("Monster number out of range");
}

std::string GetMonsterName(unsigned char c) {
  return kMonsterNames[MonsterRangeCheck(c)];
}

TileGroup GetTileGroup(unsigned char c) {
  if ((c >= 1) && (c <= 26)) {
    return kText;
  }

  if (c == 27) {
    return kKey;
  }

  if ((c >= 28) && (c <= 31)) {
    return kTreasure;
  }

  if ((c >= 32) && (c <= 63)) {
    return kText;
  }

  if (c == 64) {
    return kKeyhole;
  }

  if ((c >= 65) && (c <= 90)) {
    return kText;
  }

  if ((c >= 91) && (c <= 98)) {
    return kBrick;
  }

  if ((c >= 99) && (c <= 100)) {
    return kRope;
  }

  if ((c >= 101) && (c <= 103)) {
    return kLadder;
  }

  if (c == 108) {
    return kTeleport;
  }

  if (c == 109) {
    return kDeath;
  }

  if ((c >= 104) && (c <= 107)) {
    return kPortal;
  }

  if ((c >= 110) && (c <= 113)) {
    return kTeleport;
  }

  if ((c >= 114) && (c <= 117)) {
    return kFire;
  }

  if ((c >= 122) && (c <= 123)) {
    return kBrick;
  }

  if (c == 160) {
    return kKnownUnknown;
  }

  if (c == 243) {
    return kKnownUnknown;
  }

  return kUnknown;
}

//------------------------------------------------------------
LevelClass::LevelClass(std::span<unsigned char, kFileLength> data) {
  std::copy(data.begin(), data.end(), raw_data_.begin());

  // Clear regions that shoudn't have tiles (I found some levels have stray characxters there)
  for (int rr = 0; rr < (kTileDataRows - 1); ++rr) {
    SetTileAt(rr, 0, 32);
    SetTileAt(rr, kTileDataCols - 1, 32);
  }

  // Tile colors
  brick_color_ = raw_data_[kBrickColorCode] & 0x0f;
  ladder_color_ = raw_data_[kLadderColorCode] & 0x0f;
  rope_color_ = raw_data_[kRopeColorCode] & 0x0f;
  portal_color_ = raw_data_[kPortalColorCode] & 0x0f;
}

unsigned char LevelClass::GetTileAt(int row, int col) const {
  if ((row < 0) || (row > (kTileDataRows - 1))) {
    throw std::out_of_range("Tile column out of range");
  }

  if ((col < 0) || (col > (kTileDataCols - 1))) {
    throw std::out_of_range("Tile row out of range");
  }

  unsigned int offset = static_cast<unsigned int>(row * kTileDataCols + col);

  return raw_data_[kTileDataStart + offset];
}

unsigned char LevelClass::GetBrickColor(void) const {
  return brick_color_;
}

unsigned char LevelClass::GetLadderColor(void) const {
  return ladder_color_;
}

unsigned char LevelClass::GetRopeColor(void) const {
  return rope_color_;
}

unsigned char LevelClass::GetPortalColor(void) const {
  return portal_color_;
}

unsigned char LevelClass::GetTileColor(unsigned char c) const {
  TileGroup group = GetTileGroup(c);

  switch (group) {
    case kKnownUnknown:
      return kColorWhite;
    case kText:
      return kColorWhite;
    case kKey:
      return kColorWhite;
    case kTreasure:
      return kColorYellow;
    case kKeyhole:
      return kColorWhite;
    case kBrick:
      return brick_color_;
    case kRope:
      return rope_color_;
    case kLadder:
      return ladder_color_;
    case kPortal:
      return portal_color_;
    case kTeleport:
      return kColorCyan;
    case kDeath:
      return kColorCyan;
    case kFire:
      return kColorRed;
    default:
      std::cerr << "Unknown tile: " << std::format("{:#04x}", c) << "\n";
      return 0;
  }
}

WizardInfo LevelClass::GetWizardInfo(void) const {
  WizardInfo tmp;

  tmp.x = raw_data_[kWizardXLow];
  tmp.y = raw_data_[kWizardY];
  tmp.color = kColorPurple;

  if (raw_data_[kSpriteXHighBits] & kXMaskWizard) {
    tmp.x = tmp.x + 256;
  }

  return tmp;
}

MonsterClassArray LevelClass::GetMonsterInfo(void) const {
  MonsterData monster_data;
  MonsterClassArray tmp_monster_array;
  // TODO: check if any of the paramters are out of range

  for (unsigned int ii = 0; ii < kMaxMonsters; ++ii) {
    monster_data.id = raw_data_[kMonster0MonsterID + ii];
    monster_data.x = raw_data_[kMonster0XLow + ii];

    if (raw_data_[kSpriteXHighBits] & (1 << ii)) {
      monster_data.x = monster_data.x + 256;
    }

    monster_data.y = raw_data_[kMonster0Y + ii];
    monster_data.color = raw_data_[kMonster0ColorCode + ii];
    monster_data.sprite_id = raw_data_[kMonster0SpriteID + ii] &
                             0x7f;  // High bit needs clear as actual game sprite IDs are 128-255
    monster_data.animation_length = raw_data_[kMonster0AnimationLength + ii];

    // Elevator specific fields
    auto c = raw_data_[kElevator0DxDy + ii];
    monster_data.elevator_dx = SignExtendNibble(c >> 4);
    monster_data.elevator_dy = SignExtendNibble(c & 0x0f);
    monster_data.elevator_duration = raw_data_[kElevatorDuration];

    tmp_monster_array[ii] = MonsterClass::MonsterClassFactory(monster_data);
  }

  return tmp_monster_array;
}

MaskTiles LevelClass::GetMaskTiles(int row, int col) const {
  MaskTiles tmp;

  if ((row < 0) || (row > (kTileDataRows - 1))) {
    throw std::out_of_range("Tile column out of range");
  }

  if ((col < 0) || (col > (kTileDataCols - 1))) {
    throw std::out_of_range("Tile row out of range");
  }

  std::size_t idx = 0;

  for (int ir = 0; ir < 2; ++ir) {
    if ((row + ir) < (kTileDataRows - 1)) {
      for (int ic = 0; ic < 3; ++ic) {
        unsigned int offset = static_cast<unsigned int>((row + ir) * kTileDataCols + col + ic - 1);
        tmp[idx] = raw_data_[kTileDataStart + offset];
        idx = idx + 1;
      }
    } else {
      for (int ic = 0; ic < 3; ++ic) {
        tmp[idx] = 92;  // If below the bottom row set to floor
        idx = idx + 1;
      }
    }
  }

  return tmp;
}

void LevelClass::SetTileAt(int row, int col, unsigned char c) {
  if ((row < 0) || (row > (kTileDataRows - 1))) {
    throw std::out_of_range("Tile column out of range");
  }

  if ((col < 0) || (col > (kTileDataCols - 1))) {
    throw std::out_of_range("Tile row out of range");
  }

  unsigned int offset = static_cast<unsigned int>(row * kTileDataCols + col);

  raw_data_[kTileDataStart + offset] = c;
}

}  // namespace alatar
