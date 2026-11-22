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

#include "wizard.h"

//#include <cmath>
//#include <iostream>  // for debug
//#include <utility>

//#include "globals.h"

namespace alatar {

/*void UpdateMonsters(MonsterClassArray& monster_info, Uint64 counter) {
  for (unsigned int ii = 0; ii < 6; ++ii) {
    if (monster_info[ii]->IsActive()) {
      monster_info[ii]->Update(counter);
    }
  }
}*/

//--------------------------------------------------------------------
// WizardClass implementation
//--------------------------------------------------------------------
WizardClass::WizardClass(WizardData wizard_data) {
  x_initial_ = wizard_data.x;
  y_initial_ = wizard_data.y;
  color_ = wizard_data.color;

  x_ = wizard_data.x;
  y_ = wizard_data.y;
}

unsigned int WizardClass::GetColor(void) const {
  return color_;
}

}  // namespace alatar
