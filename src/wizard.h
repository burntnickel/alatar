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

#ifndef H_ALATAR_WIZARD
#define H_ALATAR_WIZARD

namespace alatar {

struct WizardData {
  unsigned int x;
  unsigned int y;
  unsigned int color;
};

//----------------------------------------------
// WizardClass
//----------------------------------------------
class WizardClass {
 public:
  WizardClass(void) = default;
  explicit WizardClass(WizardData wizard_data);

 public:
  unsigned int GetColor(void) const;

 public:
  // Stuff below this point is dynamic
  unsigned int x_;
  unsigned int y_;

 private:
  unsigned int x_initial_;
  unsigned int y_initial_;
  unsigned int color_;
};

// Misc. stuff
// void UpdateMonsters(MonsterClassArray& monster_info, Uint64 counter);

}  // namespace alatar

#endif
