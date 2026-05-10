#include <array>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>

enum Mode { BitMap, GrayMode };

static void DisplayUsage(std::string name) {
  std::cout << "Usage: " << name << " [--help] [-1 | -2] [-o offset] filename\n";
  std::cout << "    --help    : Displays this information\n";
  std::cout << "    -1        : Writes the output in BitMap mode assuming the charater set is\n";
  std::cout << "                high-resolution (default)\n";
  std::cout << "    -2        : Writes the output in GrayMode mode assuming the character set is\n";
  std::cout << "                in multi-color mode\n";
  std::cout << "    -o offset : Offset at which to start extracting character data\n";
  std::cout << "    filename  : The filename of the charater set to convert\n";
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

void ConvertFile(char* name, Mode mode, std::size_t offset) {
  constexpr std::size_t kCharMapLen = 2048;
  std::array<unsigned char, kCharMapLen> buffer;

  //std::cerr << "Filename: " << name << std::endl;

  std::error_code ec;
  std::uintmax_t size = std::filesystem::file_size(name, ec);

  if (ec.value() != 0) {
    std::cerr << "Error: " << ec.message() << std::endl;
  }

  //std::cerr << "File size: " << size << std::endl;

  if ((kCharMapLen + offset) > size) {
    std::cerr << "Error: Attempting to read data past end of file" << std::endl;
  }
}

int main(int argc, char* argv[]) {
  bool modeFound = false;
  int idx = 1;
  Mode mode = BitMap;
  int offset = 0;

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
    modeFound = true;
    ++idx;
  }

  if (AnyString(argc, argv, "-2")) {
    if (modeFound) {
      std::cerr << "Error: Only one of -1 or -2 may be specified" << std::endl;
      return EXIT_FAILURE;
    }

    mode = GrayMode;
    ++idx;
  }

  int position;

  if (AnyString(argc, argv, "-o", position)) {
    if (argc > (position + 1)) {
      try {
        offset = std::stoi(argv[position + 1]);
      }

      catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid offset specified" << std::endl;
        return EXIT_FAILURE;
      }

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

  ConvertFile(argv[idx], mode, offset);

  return EXIT_SUCCESS;
}
