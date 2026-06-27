#ifndef H_ALATAR_LOAD_DATA
#define H_ALATAR_LOAD_DATA

#include <filesystem>
#include <span>

namespace alatar {

bool LoadData(std::filesystem::path path_and_name, std::span<unsigned char> data,
              unsigned int data_offset = 0);

}

#endif
