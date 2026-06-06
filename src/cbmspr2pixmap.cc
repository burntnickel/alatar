#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

enum Mode { BitMap, GrayMode };

constexpr unsigned int kSpriteLen = 63;      // Size of a C64 sprite (without padding) in bytes
constexpr unsigned int kSpritePad = 1;       // Amount of inter-sprite  padding in bytes
constexpr unsigned int kCols = 3 * 8;        // Number of columns in sprint to write in pixels
constexpr unsigned int kRowsPerSprite = 21;  // Number of rows in sprint to write in pixels

static void DisplayUsage(std::string name) {
  std::cout << "Usage: " << name << " [--help] [-1 | -2] [-o offset] filename\n";
  std::cout << "    --help    : Displays this information\n";
  std::cout << "    -1        : Writes the output in BitMap mode assuming the sprite data is\n";
  std::cout << "                high-resolution (default)\n";
  std::cout << "    -2        : Writes the output in GrayMode mode assuming the sprite data is\n";
  std::cout << "                in multi-color mode\n";
  std::cout << "    -o offset : Offset at which to start extracting sprite data\n";
  std::cout << "    filename  : The filename of the sprite set to convert\n";
  std::cout << "\n";
  std::cout << "    The converted file is written to stdout" << std::endl;
}

static bool AnyString(int argc, char* argv[], const char* match, int& pos) {
  pos = -1;

  if (argc < 2) {
    return false;
  }

  for (int ii = 1; ii < argc; ++ii) {
    if (std::strcmp(argv[ii], match) == 0) {
      pos = ii;
      return true;
    }
  }

  return false;
}

static bool AnyString(int argc, char* argv[], const char* match) {
  int dummy;

  return AnyString(argc, argv, match, dummy);
}

// This function is really only useful for writing out C64 sprite bitmap data at this point and is not a
// generic PBM writer
static int GeneratePBM(const std::vector<char>& buffer, std::uint_least16_t num_sprites, std::ostream& out) {
  // Buffer size, in bits, needs to equal number of rows by columns
  if ((num_sprites * (kSpriteLen + kSpritePad)) > buffer.size()) {
    std::cerr << "Error: Buffer size < num_sprites * (kSpriteLen + kSpritePad)" << std::endl;
    return EXIT_FAILURE;
  }

  const auto rows = num_sprites * kRowsPerSprite;
  const auto col_bytes = kCols / 8;

  // Write header
  out << "P1" << "\n";
  out << "# Automatically generated PBM file\n";
  out << kCols << " " << rows << "\n";

  for (unsigned int sprite = 0; sprite < num_sprites; ++sprite) {
    for (unsigned int rr = 0; rr < kRowsPerSprite; ++rr) {
      for (unsigned int cc = 0; cc < col_bytes; ++cc) {
        auto idx = sprite * (kSpriteLen + kSpritePad) + rr * col_bytes + cc;
        auto c = buffer[idx];

        for (int bb = 0; bb < 8; ++bb) {
          if (c & 128) {
            out << "1 ";
          } else {
            out << "0 ";
          }

          c = c << 1;
        }
      }

      out << "\n";
    }
  }

  out << std::endl;

  return EXIT_SUCCESS;
}

// This function is really only useful for writing out C64 sprite bitmap data at this point and is not a
// generic PGM writer
static int GeneratePGM(const std::vector<char>& buffer, std::uint_least16_t num_sprites, std::ostream& out) {
  // Constants for the gray levels in the output file
  const unsigned int kMaxGrayVal = 255;
  const unsigned int kGrayVal00 = 0;
  const unsigned int kGrayVal01 = 85;
  const unsigned int kGrayVal10 = 170;
  const unsigned int kGrayVal11 = 255;

  // Buffer size, in bits, needs to equal number of rows by columns
  if ((num_sprites * (kSpriteLen + kSpritePad)) > buffer.size()) {
    std::cerr << "Error: Buffer size < num_sprites * (kSpriteLen + kSpritePad)" << std::endl;
    return EXIT_FAILURE;
  }

  const auto rows = num_sprites * kRowsPerSprite;
  const auto col_bytes = kCols / 8;

  // Write header
  out << "P2" << "\n";
  out << "# Automatically generated PBM file\n";
  out << kCols << " " << rows << "\n";
  out << kMaxGrayVal << "\n";

  for (unsigned int sprite = 0; sprite < num_sprites; ++sprite) {
    for (unsigned int rr = 0; rr < kRowsPerSprite; ++rr) {
      for (unsigned int cc = 0; cc < col_bytes; ++cc) {
        auto idx = sprite * (kSpriteLen + kSpritePad) + rr * col_bytes + cc;
        auto c = buffer[idx];

        for (int bb = 0; bb < 4; ++bb) {
          switch (c & (64 + 128)) {
            case 0:
              out << kGrayVal00 << " " << kGrayVal00 << " ";
              break;
            case 64:
              out << kGrayVal01 << " " << kGrayVal01 << " ";
              break;
            case 128:
              out << kGrayVal10 << " " << kGrayVal10 << " ";
              break;
            case (64 + 128):
              out << kGrayVal11 << " " << kGrayVal11 << " ";
              break;
            default:
              std::cerr << "Error: Bit sequence is odd (don't know how this could have happened)"
                        << std::endl;
              return EXIT_FAILURE;
          }

          c = c << 2;
        }
      }

      out << "\n";
    }
  }

  out << std::endl;

  return EXIT_SUCCESS;
}

static int ConvertFile(char* name, Mode mode, std::uint_least32_t offset) {
  std::vector<char> buffer;

  std::error_code ec;
  std::uintmax_t size = std::filesystem::file_size(name, ec);

  if (ec.value() != 0) {
    std::cerr << "Error: " << ec.message() << std::endl;
  }

  std::uintmax_t buffer_size = size - offset;

  std::uint_least16_t num_sprites =
      static_cast<std::uint_least16_t>((buffer_size) / (kSpriteLen + kSpritePad));

  // Ensure the buffer is of the correct size
  buffer.resize(buffer_size);

  std::ifstream in(name, std::ios::binary);

  if (!in.is_open()) {
    std::cerr << "Error: Unable to open file" << std::endl;
    return EXIT_FAILURE;
  }

  in.seekg(offset, std::ios::beg);

  in.read(buffer.data(), static_cast<std::streamsize>(buffer_size));

  if (in.gcount() != static_cast<std::streamsize>(buffer_size)) {
    std::cerr << "Error: Unexpected end of file" << std::endl;
    return EXIT_FAILURE;
  }

  in.close();

  int return_code;

  switch (mode) {
    case BitMap:
      return_code = GeneratePBM(buffer, num_sprites, std::cout);
      break;

    case GrayMode:
      return_code = GeneratePGM(buffer, num_sprites, std::cout);
      break;

    default:
      std::cerr << "Error: Unknown mode specified" << std::endl;
      return EXIT_FAILURE;
  }
  return return_code;
}

int main(int argc, char* argv[]) {
  bool mode_found = false;
  int idx = 1;
  Mode mode = BitMap;
  std::uint_least32_t offset = 0;

  if (argc < 2) {
    std::cerr << "No arguments supplied. Type \"" << argv[0] << " --help\" for usage." << std::endl;
    return EXIT_FAILURE;
  }

  if (AnyString(argc, argv, "--help")) {
    DisplayUsage(argv[0]);
    return EXIT_SUCCESS;
  }

  if (AnyString(argc, argv, "-1")) {
    mode = BitMap;
    mode_found = true;
    ++idx;
  }

  if (AnyString(argc, argv, "-2")) {
    if (mode_found) {
      std::cerr << "Error: Only one of -1 or -2 may be specified" << std::endl;
      return EXIT_FAILURE;
    }

    mode = GrayMode;
    ++idx;
  }

  int position;
  long signed_offset;

  if (AnyString(argc, argv, "-o", position)) {
    if (argc > (position + 1)) {
      try {
        signed_offset = std::stol(argv[position + 1]);
      }

      catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid offset specified" << std::endl;
        return EXIT_FAILURE;
      }

      if (signed_offset < 0) {
        std::cerr << "Error: Offset must be non-negative" << std::endl;
        return EXIT_FAILURE;
      }

      offset = static_cast<std::uint_least32_t>(signed_offset);
      idx += 2;
    } else {
      std::cerr << "Error: No offset provided" << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (idx >= argc) {
    std::cerr << "Error: No valid input filename provided" << std::endl;
    return EXIT_FAILURE;
  }

  return ConvertFile(argv[idx], mode, offset);
}
