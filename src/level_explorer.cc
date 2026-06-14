#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

#include "wiz_level.h"

using namespace wizard_level;

static void ScreenToAsciiHelper(std::map<unsigned char, std::string>& m, std::string_view chars,
                                unsigned char start) {
  for (unsigned char ii = 0; ii < chars.size(); ++ii) {
    m[start + ii] = chars[ii];
  }
}

static void InitScreenToAscii(std::map<unsigned char, std::string>& m) {
  const std::string lower_case{"abcdefghijklmnopqrstuvwxyz"};
  const std::string upper_case{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
  const std::string numbers{"0123456789"};
  const std::string symbols1{" !\"#$%&\'()*+,-./"};
  const std::string symbols2{":;<=>\?"};

  m[0] = '@';

  ScreenToAsciiHelper(m, lower_case, 1);
  ScreenToAsciiHelper(m, upper_case, 65);
  ScreenToAsciiHelper(m, numbers, 48);
  ScreenToAsciiHelper(m, symbols1, 32);
  ScreenToAsciiHelper(m, symbols2, 58);

  m[27] = '[';
  m[28] = "\u00A3";  // unicode pound symbol
  m[29] = ']';
  m[30] = "\u2191";  // unicode up arrow
  m[31] = "\u2190";   // unicode left arrow
}

static std::string ScreenCodeToASCII(unsigned char c) {
  static std::map<unsigned char, std::string> screen_to_ascii;
  static bool init = false;

  if (!init) {
    init = true;
    InitScreenToAscii(screen_to_ascii);
  }

  try {
    return screen_to_ascii.at(c);
  }

  catch (const std::out_of_range& e) {
    throw std::out_of_range("Screen code out of range: " + std::to_string(static_cast<unsigned int>(c)));
  }
}

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
      std::cout << "Monster " << n << " Color Code (" << GetC64Color(c) << ")\n";
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
      std::cout << "Monster " << n << " ID (" << GetMonsterName(c) << ")\n";
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
        std::cout << "Brick Color Code (" << GetC64Color(c) << ")";
        break;
      case kLadderColorCode:
        std::cout << "Ladder Color Code (" << GetC64Color(c) << ")";
        break;
      case kRopeColorCode:
        std::cout << "Rope Color Code (" << GetC64Color(c) << ")";
        break;
      case kPortalColorCode:
        std::cout << "Portal Color Code (" << GetC64Color(c) << ")";
        break;
      case kSpellNumber:
        std::cout << "Spell Number (" << GetSpellName(c) << ")";
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
        std::cout << "Spell Color Code (" << GetC64Color(c) << ")";
        break;
      case kSpriteXHighBits:
        std::cout << "Sprite X-position (high bits, " << std::format("{:#010b}", c) << ")";
        break;
      case kSafeColorCode:
        std::cout << "Safe Color Code (" << GetC64Color(c) << ")";
        break;
      default:
        std::cout << "unknown";
    }

    std::cout << "\n";
  }
}

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

  DisplayLevelInfo(buffer);

  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Exactly one arguement must be supplied, the file name" << std::endl;
    return EXIT_FAILURE;
  }

  return ExploreLevel(argv[1]);
}
