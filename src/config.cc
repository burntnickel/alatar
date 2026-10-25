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

#include "globals.h"
#include "pathdef.h"

namespace alatar {

std::filesystem::path GetClutFilePath(std::string_view section, std::string_view key,
                                      std::filesystem::path resource_path) {
  BurningLogic::IniFile::ErrorCondition ec;
  auto clut_filename_opt = alatar::gIniFile.GetAsString(section, key, ec);

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

}  // namespace alatar

