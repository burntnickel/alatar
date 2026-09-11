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

#include "util.h"

namespace alatar {

int SignExtendNibble(unsigned char c) {
  c = c & 0x0f;

  if (c <= 7) {
    return static_cast<int>(c);
  }

  return -1 * (16 - static_cast<int>(c));
}

}  // namespace alatar
