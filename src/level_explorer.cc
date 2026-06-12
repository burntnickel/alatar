#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <span>
#include <string>

// Move a bunch of this to a header and its own .cc file
namespace wizard_level {

constexpr int kFileLength = 1138;

enum {
  kMonster0XLow = 2,
  kMonster1XLow = 3,
  kMonster2XLow = 4,
  kMonster3XLow = 5,
  kMonster4XLow = 6,
  kMonster5XLow = 7,
  kMonster0Y = 8,
  kMonster1Y = 9,
  kMonster2Y = 10,
  kMonster3Y = 11,
  kMonster4Y = 12,
  kMonster5Y = 13,
  kBrickColorCode = 14,
  kLadderColorCode = 15,
  kRopeColorCode = 16,
  kPortalColorCode = 17,
  kMonster0ColorCode = 18,
  kMonster1ColorCode = 19,
  kMonster2ColorCode = 20,
  kMonster3ColorCode = 21,
  kMonster4ColorCode = 22,
  kMonster5ColorCode = 23
};

enum { kSpellNumber = 30, kSpellCount = 31, kWizardXLow = 32, kWizardY = 33 };

enum { kBonusSpeed = 82, kSpellColorCode = 83, kSpriteXHighBits = 84 };

enum {
  kSlide0StartTileLowByte = 85,
  kSlide1StartTileLowByte = 86,
  kSlide2StartTileLowByte = 87,
  kSlide0StartTileHighByte = 88,
  kSlide1StartTileHighByte = 89,
  kSlide2StartTileHighByte = 90,
  kSlide0EndTile = 91,
  kSlide1EndTile = 92,
  kSlide2EndTile = 93,
  kSlide0Time = 94,
  kSlide1Time = 95,
  kSlide2Time = 96
};

const unsigned int kSlideTileBase = 0xc400;

enum {
  kMonster0SpriteID = 98,
  kMonster1SpriteID = 99,
  kMonster2SpriteID = 100,
  kMonster3SpriteID = 101,
  kMonster4SpriteID = 102,
  kMonster5SpriteID = 103
};

enum {
  kMonster0AnimationLength = 106,
  kMonster1AnimationLength = 107,
  kMonster2AnimationLength = 108,
  kMonster3AnimationLength = 109,
  kMonster4AnimationLength = 110,
  kMonster5AnimationLength = 111
};

enum {
  kSafeColorCode = 113,
  kMonster0MonsterID = 114,
  kMonster1MonsterID = 115,
  kMonster2MonsterID = 116,
  kMonster3MonsterID = 117,
  kMonster4MonsterID = 118,
  kMonster5MonsterID = 119
};

enum { kTileDataStart = 299, kTileDataEnd = 1096 };

enum { kLevelNameStart = 1106, kLevelNameEnd = 1129 };

/*struct MosterInfo {
  int initial_pos_x;
  int initial_pos_y;
  int color_code;
  int sprite_number;
  int animation_length;
  int monster_id;
};*/

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

static unsigned char SpellRangeCheck(unsigned char c) {
  if (c < kSpellNames.size()) {
    return c;
  }

  throw std::out_of_range("Spell number out of range");
}

static unsigned char MonsterRangeCheck(unsigned char c) {
  if (c < kMonsterNames.size()) {
    return c;
  }

  throw std::out_of_range("Monster number out of range");
}

static char ScreenCodeToASCII(unsigned char c) {
  const char lower_case[] = "abcdefghijklmnopqrstuvwxyz";
  const char upper_case[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  const char numbers[] = "0123456789";
  const char symbols1[] = "[\u00A3]\u2191\u2190 !\"#$%&\'()*+,-./";
  const char symbols2[] = ":;<=>\?";

  if (c == 0) {
    return '@';
  }

  if ((c >= 1) && (c <= 26)) {
    return lower_case[c - 1];
  }

  if ((c >= 65) && (c <= 90)) {
    return upper_case[c - 65];
  }

  if ((c >= 48) && (c <= 57)) {
    return numbers[c - 48];
  }

  if ((c >= 27) && (c <= 47)) {
    return symbols1[c - 27];
  }

  if ((c >= 58) && (c <= 63)) {
    return symbols2[c - 58];
  }

  throw std::out_of_range("Screen code out of range: " + std::to_string(static_cast<unsigned int>(c)));
}

// Use if staments to split up by range
static void DisplayLevelInfo(std::span<const char, kFileLength> data) {
  for (unsigned int ii = 0; ii < kFileLength; ++ii) {
    unsigned char c = static_cast<unsigned char>(data[ii]);

    std::cout << std::format("{:#06x}", ii) << "    " << std::format("{:#04x}", c) << "    ; ";

    if ((ii >= kMonster0XLow) && (ii <= kMonster5XLow)) {
      int n = static_cast<int>(ii - kMonster0XLow);
      std::cout << "Monster " << n << " X-position (low 8 bits)\n";
      continue;
    }

    if ((ii >= kMonster0Y) && (ii <= kMonster5Y)) {
      int n = static_cast<int>(ii - kMonster0Y);
      std::cout << "Monster " << n << " Y-position\n";
      continue;
    }

    if ((ii >= kMonster0ColorCode) && (ii <= kMonster5ColorCode)) {
      int n = static_cast<int>(ii - kMonster0ColorCode);
      std::cout << "Monster " << n << " Color Code (" << kColorStrings[ColorRangeCheck(c)] << ")\n";
      continue;
    }

    if ((ii >= kSlide0StartTileLowByte) && (ii <= kSlide2StartTileLowByte)) {
      int n = static_cast<int>(ii - kSlide0StartTileLowByte);
      std::cout << "Slide " << n << " Start Tile Offset (low byte)\n";
      continue;
    }

    if ((ii >= kSlide0StartTileHighByte) && (ii <= kSlide2StartTileHighByte)) {
      int n = static_cast<int>(ii - kSlide0StartTileHighByte);
      std::cout << "Slide " << n << " Start Tile Offset (high byte)\n";
      continue;
    }

    if ((ii >= kSlide0EndTile) && (ii <= kSlide2EndTile)) {
      int n = static_cast<int>(ii - kSlide0EndTile);
      std::cout << "Slide " << n << " End Tile Offset\n";
      continue;
    }

    if ((ii >= kSlide0Time) && (ii <= kSlide2Time)) {
      int n = static_cast<int>(ii - kSlide0Time);
      int tSlide = c >> 4;
      int tNormal = c & 0x0f;
      std::cout << "Slide " << n << " Time slide/normal (" << tSlide << "/" << tNormal << ")\n";
      continue;
    }

    if ((ii >= kMonster0SpriteID) && (ii <= kMonster5SpriteID)) {
      int n = static_cast<int>(ii - kMonster0SpriteID);
      std::cout << "Monster " << n << " Sprite ID\n";
      continue;
    }

    if ((ii >= kMonster0AnimationLength) && (ii <= kMonster5AnimationLength)) {
      int n = static_cast<int>(ii - kMonster0AnimationLength);
      std::cout << "Monster " << n << " Animation Length\n";
      continue;
    }

    if ((ii >= kMonster0MonsterID) && (ii <= kMonster5MonsterID)) {
      int n = static_cast<int>(ii - kMonster0MonsterID);
      std::cout << "Monster " << n << " ID (" << kMonsterNames[MonsterRangeCheck(c)] << ")\n";
      continue;
    }

    if ((ii >= kTileDataStart) && (ii <= kTileDataEnd)) {
      std::cout << "Level Tile Data\n";
      continue;
    }

    if ((ii >= kLevelNameStart) && (ii <= kLevelNameEnd)) {
      std::cout << "Level Name (" << ScreenCodeToASCII(c) << ")\n";
      continue;
    }

    switch (ii) {
      case kBrickColorCode:
        std::cout << "Brick Color Code (" << kColorStrings[ColorRangeCheck(c)] << ")";
        break;
      case kLadderColorCode:
        std::cout << "Ladder Color Code (" << kColorStrings[ColorRangeCheck(c)] << ")";
        break;
      case kRopeColorCode:
        std::cout << "Rope Color Code (" << kColorStrings[ColorRangeCheck(c)] << ")";
        break;
      case kPortalColorCode:
        std::cout << "Portal Color Code (" << kColorStrings[ColorRangeCheck(c)] << ")";
        break;
      case kSpellNumber:
        std::cout << "Spell Number (" << kSpellNames[SpellRangeCheck(c)] << ")";
        break;
      case kSpellCount:
        std::cout << "Spell Count (" << c << ")";  // Spell count is stored in ASCII for some reason
        break;
      case kWizardXLow:
        std::cout << "Wizard X-position (low 8 bits)";
        break;
      case kWizardY:
        std::cout << "Wizard Y-position";
        break;
      case kBonusSpeed:
        std::cout << "Bonus Countdown Speed";
        break;
      case kSpellColorCode:
        std::cout << "Spell Color Code (" << kColorStrings[ColorRangeCheck(c)] << ")";
        break;
      case kSpriteXHighBits:
        std::cout << "Sprite X-position (high bits, " << std::format("{:#010b}", c) << ")";
        break;
      case kSafeColorCode:
        std::cout << "Safe Color Code (" << kColorStrings[ColorRangeCheck(c)] << ")";
        break;
      default:
        std::cout << "unknown";
    }

    std::cout << "\n";
  }
}

}  // namespace wizard_level

static int ExploreLevel(char* name) {
  std::error_code ec;
  std::uintmax_t size = std::filesystem::file_size(name, ec);

  if (ec.value() != 0) {
    std::cerr << "Error: " << ec.message() << std::endl;
    return EXIT_FAILURE;
  }

  if (size != wizard_level::kFileLength) {
    std::cerr << "Error: File is of the incorrect size" << std::endl;
    return EXIT_FAILURE;
  }

  std::array<char, wizard_level::kFileLength> buffer;

  std::ifstream in(name, std::ios::binary);

  if (!in.is_open()) {
    std::cerr << "Error: Unable to open file" << std::endl;
    return EXIT_FAILURE;
  }

  in.read(buffer.data(), wizard_level::kFileLength);

  if (in.gcount() != wizard_level::kFileLength) {
    std::cerr << "Error: Unexpected end of file" << std::endl;
    return EXIT_FAILURE;
  }

  in.close();

  wizard_level::DisplayLevelInfo(buffer);

  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Exactly one arguement must be supplied, the file name" << std::endl;
    return EXIT_FAILURE;
  }

  return ExploreLevel(argv[1]);
}
