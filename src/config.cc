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

#include "config.h"

#include "pathdef.h"

namespace alatar {

std::filesystem::path GetClutFilePath(BurningLogic::IniFile ini_file, std::string_view section,
                                      std::string_view key, std::filesystem::path resource_path) {
  BurningLogic::IniFile::ErrorCondition ec;
  auto clut_filename_opt = ini_file.GetAsString(section, key, ec);

  std::filesystem::path clut_path_file;

  if (clut_filename_opt) {
    std::string clut_filename = clut_filename_opt.value();
    clut_path_file = resource_path / alatar::kClutDirName / clut_filename;
  } else {
    // Using an empty string results in being provided the default CLUT
    std::cout << "Unable to get 'palette_file' from config.ini, using default\n";
    clut_path_file = "";
  }

  return clut_path_file;
}

float GetFloatFromIni(BurningLogic::IniFile ini_file, std::string_view section, std::string_view key,
                      float default_value) {
  BurningLogic::IniFile::ErrorCondition ec;
  auto double_opt = ini_file.GetAsDouble(section, key, ec);

  if (ec == BurningLogic::IniFile::NO_ERROR) {
    float tmp = static_cast<float>(double_opt.value());
    std::cout << "Over riding " << section << ":" << key << " " << default_value << " => " << tmp << "\n";
    return tmp;
  }

  return default_value;
}

}  // namespace alatar
