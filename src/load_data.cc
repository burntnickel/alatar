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
