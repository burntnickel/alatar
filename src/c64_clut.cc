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

#include "c64_clut.h"

#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace alatar {

// This is the default CLUT (I think I got it from Wikipedia)
C64ClutTable kDefaultC64Clut{{
    0x00, 0x00, 0x00, 0xff,  // black
    0xFF, 0xFF, 0xFF, 0xff,  // white
    0x68, 0x37, 0x2B, 0xff,  // red
    0x70, 0xA4, 0xB2, 0xff,  // light blue (cyan?)
    0x6F, 0x3D, 0x86, 0xff,  // purple
    0x58, 0x8D, 0x43, 0xff,  // green
    0x35, 0x28, 0x79, 0xff,  // dark blue
    0xB8, 0xC7, 0x6F, 0xff,  // yellow
    0x6F, 0x4F, 0x25, 0xff,  // brown (orange?)
    0x43, 0x39, 0x00, 0xff,  // dark brown
    0x9A, 0x67, 0x59, 0xff,  // light red (pink?)
    0x44, 0x44, 0x44, 0xff,  // dark grey
    0x6C, 0x6C, 0x6C, 0xff,  // mid grey
    0x9A, 0xD2, 0x84, 0xff,  // light green
    0x6C, 0x5E, 0xB5, 0xff,  // mid blue
    0x95, 0x95, 0x95, 0xff   // light grey
}};

std::optional<C64ClutTable> LoadClutTable(std::filesystem::path clut_path) {
  std::ifstream in(clut_path);
  std::error_code ec;
  bool done_reading = false;
  C64ClutTable tmp_clut;
  unsigned int clut_index = 0;

  if (!in.is_open()) {
    std::cerr << "Error: " << std::strerror(errno) << "(" << clut_path << ")" << std::endl;
    return std::nullopt;
  }

  while (!done_reading) {
    std::string next_line;

    std::getline(in, next_line);

    if (!in) {
      done_reading = true;
    } else {
      std::istringstream iss(next_line);
      std::string sub_string;
      std::vector<std::string> tokens;
      tokens.clear();

      while (iss >> sub_string) {
        tokens.push_back(sub_string);
      }

      auto num_tokens = tokens.size();

      if (num_tokens > 0) {
        if (tokens[0][0] == '#') {
          // Skip comment lines
        } else {
          if (num_tokens != 3) {
            // All processed lines should have exactly three tokens, the RGB values
            std::cerr << "Error: unexpected number of tokens found reading file : '" << next_line << "'"
                      << std::endl;
            return std::nullopt;
          }

          std::array<unsigned char, 3> rgb_data;

          for (unsigned int ii = 0; ii < 3; ++ii) {
            try {
              unsigned long tmp_val = std::stoul(tokens[ii], 0, 16);

              if (tmp_val > 255) {
                std::cerr << "Error: RGB component > 255 (0xff)" << std::endl;
                return std::nullopt;
              }

              rgb_data[ii] = static_cast<unsigned char>(tmp_val);
            }

            catch (const std::invalid_argument& e) {
              std::cerr << "Error: could not convert string" << std::endl;
              return std::nullopt;
            }

            catch (const std::out_of_range& e) {
              std::cerr << "Error: out of range" << std::endl;
              return std::nullopt;
            }
          }

          if (clut_index > 15) {
            // There should only be entries, if we're here there are too many
            std::cerr << "Error: too many entries in palette file" << std::endl;
            return std::nullopt;
          }

          for (unsigned int ii = 0; ii < 3; ++ii) {
            unsigned int idx = 4 * clut_index + ii;
            tmp_clut[idx] = rgb_data[ii];
          }

          tmp_clut[4 * clut_index + 3] = 0xff;

          ++clut_index;
        }
      }
    }
  }

  in.close();

  // Need to check to see if we had too few entries as well
  if (clut_index < 16) {
    std::cerr << "Error: too few entries in palette file" << std::endl;
    return std::nullopt;
  }

  return tmp_clut;
}

C64ClutTable GetClutTable(std::filesystem::path clut_path) {
  if (clut_path.empty()) {
    return kDefaultC64Clut;
  }

  auto the_clut = LoadClutTable(clut_path);

  if (the_clut) {
    return the_clut.value();
  }

  std::cerr << "Unable to load CLUT, using default\n";

  return kDefaultC64Clut;
}

}  // namespace alatar
