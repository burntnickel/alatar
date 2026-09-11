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

#ifndef H_ALATAR_LOAD_DATA
#define H_ALATAR_LOAD_DATA

#include <filesystem>
#include <span>

namespace alatar {

// Number of bytes to skip in files starting with a loading address
constexpr unsigned int kLoadAddressOffset = 2;

bool LoadData(std::filesystem::path path_and_name, std::span<unsigned char> data,
              unsigned int data_offset = 0);

}  // namespace alatar

#endif
