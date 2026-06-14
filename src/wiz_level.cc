#include <algorithm>
#include <array>
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

//------------------------------------------------------------
LevelClass::LevelClass(std::span<unsigned char, kFileLength> data) {
  std::copy(data.begin(), data.end(), raw_data_.begin());

  // Tile colors
  brick_color_ = raw_data_[kBrickColorCode];
  ladder_color_ = raw_data_[kLadderColorCode];
  rope_color_ = raw_data_[kRopeColorCode];
  portal_color_ = raw_data_[kPortalColorCode];
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

}  // namespace wizard_level
