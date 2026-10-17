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

#ifndef H_ALATAR_C64_CLUT
#define H_ALATAR_C64_CLUT

#include <array>
#include <filesystem>
#include <optional>

namespace alatar {

using C64ClutTable = std::array<unsigned char, 4 * 16>;

// Ideally this would be expected rather than optional but we'd need to complie with a newer C++ standard
std::optional<C64ClutTable> LoadClutTable(std::filesystem::path clut_path);

// If the path is empty the function returns the default CLUT
C64ClutTable GetClutTable(std::filesystem::path clut_path = "");

}  // namespace alatar

#endif
