// Copyright 2026 Jude Giampaolo
//
// This file is part of Alatar.
//
// Alatar is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Alatar is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Alatar.
// If not, see <https://www.gnu.org/licenses/>. 

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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

// This function is really only useful for writing out C64 bitmap data at this point and is not a generic PBM
// writer
static int GeneratePBM(const std::vector<char>& buffer, std::uint_least16_t cols, std::uint_least16_t rows,
                       std::ostream& out) {
  // Buffer size, in bits, needs to equal number of rows by columns
  if (rows * cols != 8 * buffer.size()) {
    std::cerr << "Error: Buffer size != rows * cols" << std::endl;
    return EXIT_FAILURE;
  }

  if (rows % 8 != 0) {
    std::cerr << "Error: Number of rows must be divisible by 8" << std::endl;
    return EXIT_FAILURE;
  }

  if (cols % 8 != 0) {
    std::cerr << "Error: Number of columns must be divisible by 8" << std::endl;
    return EXIT_FAILURE;
  }

  // Write header
  out << "P1" << "\n";
  out << "# Automatically generated PBM file\n";
  out << cols << " " << rows << "\n";

  unsigned int col_chars = cols / 8;

  // This includes all of the funny decoding of the byte ordering
  for (unsigned int rr = 0; rr < rows; ++rr) {
    for (unsigned int cc = 0; cc < col_chars; ++cc) {
      unsigned int idx = cols * (rr / 8) + 8 * cc + (rr & 7);
      char c = buffer[idx];

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

  out << std::endl;

  return EXIT_SUCCESS;
}

// This function is really only useful for writing out C64 bitmap data at this point and is not a generic PBM
// writer (this version assumes multi-color characters)
static int GeneratePGM(const std::vector<char>& buffer, std::uint_least16_t cols, std::uint_least16_t rows,
                       std::ostream& out) {
  // Constants for the gray levels in the output file
  const unsigned int kMaxGrayVal = 255;
  const unsigned int kGrayVal00 = 0;
  const unsigned int kGrayVal01 = 85;
  const unsigned int kGrayVal10 = 170;
  const unsigned int kGrayVal11 = 255;

  // Buffer size, in bits, needs to equal number of rows by columns
  if (rows * cols != 8 * buffer.size()) {
    std::cerr << "Error: Buffer size != rows * cols" << std::endl;
    return EXIT_FAILURE;
  }

  if (rows % 8 != 0) {
    std::cerr << "Error: Number of rows must be divisible by 8" << std::endl;
    return EXIT_FAILURE;
  }

  if (cols % 8 != 0) {
    std::cerr << "Error: Number of columns must be divisible by 8" << std::endl;
    return EXIT_FAILURE;
  }

  // Write header
  out << "P2" << "\n";
  out << "# Automatically generated PBM file\n";
  out << cols << " " << rows << "\n";
  out << kMaxGrayVal << "\n";

  unsigned int col_chars = cols / 8;

  // This includes all of the funny decoding of the byte ordering
  for (unsigned int rr = 0; rr < rows; ++rr) {
    for (unsigned int cc = 0; cc < col_chars; ++cc) {
      unsigned int idx = cols * (rr / 8) + 8 * cc + (rr & 7);
      char c = buffer[idx];

      // We write out each pixel twice as C64 pixels are double with in 2-bit / multicolor mode
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
            std::cerr << "Error: Bit sequence is odd (don't know how this could have happened)" << std::endl;
            return EXIT_FAILURE;
        }

        c = c << 2;
      }
    }

    out << "\n";
  }

  out << std::endl;

  return EXIT_SUCCESS;
}

static int ConvertFile(char* name, Mode mode, std::uint_least32_t offset) {
  constexpr std::uint_least32_t kCharMapLen = 2048;  // Size of a C64 charater set in bytes
  constexpr std::uint_least16_t kCols = 16 * 8;      // Number of columns in pixmap to write in pixels
  constexpr std::uint_least16_t kRows = 16 * 8;      // Number of rows in pixmap to write in pixels
  std::vector<char> buffer;

  // Ensure the buffer is ofte correct size (I would like to use std::array but then it doesn't know its size
  // so passing generically to functions is an issue)
  buffer.resize(kCharMapLen);

  std::error_code ec;
  std::uintmax_t size = std::filesystem::file_size(name, ec);

  if (ec.value() != 0) {
    std::cerr << "Error: " << ec.message() << std::endl;
  }

  if ((kCharMapLen + offset) > size) {
    std::cerr << "Error: Attempting to read more data than in file" << std::endl;
    return EXIT_FAILURE;
  }

  std::ifstream in(name, std::ios::binary);

  if (!in.is_open()) {
    std::cerr << "Error: Unable to open file" << std::endl;
    return EXIT_FAILURE;
  }

  in.seekg(offset, std::ios::beg);

  in.read(buffer.data(), kCharMapLen);

  if (in.gcount() != kCharMapLen) {
    std::cerr << "Error: Unexpected end of file" << std::endl;
    return EXIT_FAILURE;
  }

  in.close();

  int return_code;

  switch (mode) {
    case BitMap:
      return_code = GeneratePBM(buffer, kCols, kRows, std::cout);
      break;

    case GrayMode:
      return_code = GeneratePGM(buffer, kCols, kRows, std::cout);
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
