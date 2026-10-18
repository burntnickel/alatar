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

#ifndef H_ALATAR_PATHDEF
#define H_ALATAR_PATHDEF

#include <string>

namespace alatar {

// Directory name for the OpenGL shaders
const std::string kShadersDirName{"shaders"};

// Directory name for color lookup tables / palettes
const std::string kClutDirName{"palettes"};

// Directory name for classic mode resources
const std::string kClassicDirName{"classic"};

// Directory name for updated mode resources
const std::string kUpdatedDirName{"updated"};

}  // namespace alatar

#endif
