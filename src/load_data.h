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
