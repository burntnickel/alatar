#include <algorithm>
#include <array>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

#include "wiz_level.h"

namespace wizard_level {

std::array<const std::string, 16> kColorStrings = {{"black", "white", "red", "cyan", "purple", "green",
                                                    "blue", "yellow", "orange", "brown", "pink", "dark grey",
                                                    "grey", "light green", "light blue", "light grey"}};

std::array<const std::string, 12> kSpellNames = {{"Fire Ball", "Magic Missile", "Disintegrate", "Enchantment",
                                                  "Freeze", "Invisibility", "Teleport", "Feather Fall",
                                                  "Levitate", "Haste", "Slow", "none"}};

std::array<const std::string, 21> kMonsterNames = {
    {"none",      "Arrow",    "Bat",   "Ghost",        "Evil Wizard",  "Witch",      "Falling Rock",
     "Elevator",  "Lava",     "Pit",   "Trap Door",    "Sliding Gate", "Lava Troll", "Rolling Rock",
     "Giant Rat", "Scorpion", "Slime", "Giant Spider", "Shadow Lord",  "Theif",      "Wizard's Cat"}};

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

  return kUnknown;
}

//------------------------------------------------------------
LevelClass::LevelClass(std::span<unsigned char, kFileLength> data) {
  std::copy(data.begin(), data.end(), raw_data_.begin());

  // Tile colors
  brick_color_ = raw_data_[kBrickColorCode] & 0x0f;
  ladder_color_ = raw_data_[kLadderColorCode] & 0x0f;
  rope_color_ = raw_data_[kRopeColorCode] & 0x0f;
  portal_color_ = raw_data_[kPortalColorCode] & 0x0f;
  std::cout << "brick color: " << std::format("{:#04x}", brick_color_) << "\n";
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
      return 2;  // red
    default:
      std::cerr << "Unknown tile: " << std::format("{:#04x}", c) << "\n";
      return 0;
  }
}

}  // namespace wizard_level
