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

#include <fstream>
#include <iostream>

#include "load_data.h"

namespace alatar {

bool LoadData(std::filesystem::path path_and_name, std::span<unsigned char> data,
              unsigned int data_offset) {
  std::error_code ec;
  std::uintmax_t size = std::filesystem::file_size(path_and_name, ec);

  if (ec.value() != 0) {
    std::cerr << "Error: " << ec.message() << "(" << path_and_name << ")" << std::endl;
    return false;
  }

  auto data_size = data.size();

  if (size != (data_size + data_offset)) {
    std::cerr << "Error: File is of the incorrect size" << std::endl;
    return false;
  }

  std::ifstream in(path_and_name, std::ios::binary);

  if (!in.is_open()) {
    std::cerr << "Error: Unable to open file" << std::endl;
    return false;
  }

  in.seekg(data_offset, std::ios::beg);

  in.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data_size));

  if (in.gcount() != static_cast<std::streamsize>(data_size)) {
    std::cerr << "Error: Unexpected end of file" << std::endl;
    return false;
  }

  in.close();

  return true;
}

}  // namespace alatar
