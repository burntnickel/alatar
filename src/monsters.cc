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

#include "monsters.h"

#include <cmath>
#include <iostream>  // for debug
#include <utility>

#include "globals.h"

namespace alatar {

void UpdateMonsters(MonsterClassArray& monster_info, Uint64 counter) {
  for (unsigned int ii = 0; ii < 6; ++ii) {
    if (monster_info[ii]->IsActive()) {
      monster_info[ii]->Update(counter);
    }
  }
}

//--------------------------------------------------------------------
// MonsterClass implementation
//--------------------------------------------------------------------
MonsterClass::MonsterClass(MonsterData monster_data) {
  // TODO: Add validation here
  active_ = (monster_data.id != kNone);
  id_ = monster_data.id;
  x_initial_ = monster_data.x;
  y_initial_ = monster_data.y;
  color_ = monster_data.color;
  sprite_id_initial_ = monster_data.sprite_id;
  animation_length_ = monster_data.animation_length;
  priority_ = true;

  x_ = monster_data.x;
  y_ = monster_data.y;
  x_float_ = static_cast<float>(monster_data.x);
  y_float_ = static_cast<float>(monster_data.y);
  sprite_id_ = monster_data.sprite_id;
};

bool MonsterClass::IsActive(void) const {
  return active_;
}

unsigned int MonsterClass::GetId(void) const {
  return id_;
}

unsigned int MonsterClass::GetXInitial(void) const {
  return x_initial_;
}

unsigned int MonsterClass::GetYInitial(void) const {
  return y_initial_;
}

unsigned int MonsterClass::GetColor(void) const {
  return color_;
}

unsigned int MonsterClass::GetSpriteIDInitial(void) const {
  return sprite_id_initial_;
}

unsigned int MonsterClass::GetAnimationLength(void) const {
  return animation_length_;
}

bool MonsterClass::GetPriority(void) const {
  return priority_;
}

int MonsterClass::GetSpriteMods(void) const {
  return sprite_mods_;
}

void MonsterClass::Update(Uint64 counter) {
  if (old_counter_valid_) {
    elapsed_ms_ = static_cast<float>(counter - old_counter_) * static_cast<float>(alatar::gCounterToMsScale);
  } else {
    elapsed_ms_ = 0.0f;
    old_counter_valid_ = true;
  }

  old_counter_ = counter;

  // Adjust animation sprite ID
  animation_counter_ = animation_counter_ + frames_per_ms_ * elapsed_ms_;
  animation_state_ = static_cast<unsigned int>(animation_counter_);

  if (animation_state_ >= animation_length_) {
    animation_counter_ = animation_counter_ - static_cast<float>(animation_length_);
    animation_state_ = 0;

    // If we're really running behind the animation counter will be greater than 1 at this point so we need to
    // check and correct
    if (animation_counter_ >= 1.0f) {
      animation_counter_ = 0.0f;
    }
  }

  sprite_id_ = sprite_id_initial_ + animation_state_;
}

MonsterClassPtr MonsterClass::MonsterClassFactory(MonsterData monster_data) {
  switch (monster_data.id) {
    case kElevator:
      return std::make_unique<ElevatorClass>(monster_data);
      break;
    case kLava:
      return std::make_unique<LavaClass>(monster_data);
      break;
    case kSlidingGate:
      return std::make_unique<SlidingGateClass>(monster_data);
      break;
    case kLavaTroll:
      return std::make_unique<LavaTrollClass>(monster_data);
      break;
    default:
      return std::make_unique<MonsterClass>(monster_data);
  }
}

//--------------------------------------------------------------------
// Elevator implementation
//--------------------------------------------------------------------
ElevatorClass::ElevatorClass(MonsterData monster_data) : MonsterClass(monster_data) {
  priority_ = true;
  sprite_mods_ = alatar_classic::kSpriteMultiColor;
  x_delta_ = static_cast<float>(monster_data.elevator_dx);
  y_delta_ = static_cast<float>(monster_data.elevator_dy);
  duration_ = static_cast<float>(monster_data.elevator_duration);
  frames_per_ms_ = kFramePerMs;
}

void ElevatorClass::Update(Uint64 counter) {
  MonsterClass::Update(counter);

  if (tick_up_) {
    ticks_ = ticks_ + elapsed_ms_ * kTicksPerMs * kSpeedFactor;

    if (ticks_ > duration_) {
      ticks_ = duration_;
      tick_up_ = false;
    }
  } else {
    ticks_ = ticks_ - elapsed_ms_ * kTicksPerMs * kSpeedFactor;

    if (ticks_ < 0.0f) {
      ticks_ = 0.0f;
      tick_up_ = true;
    }
  }

  x_float_ = static_cast<float>(x_initial_) + x_delta_ * ticks_;
  y_float_ = static_cast<float>(y_initial_) + y_delta_ * ticks_;

  x_ = static_cast<unsigned int>(std::round(x_float_));
  y_ = static_cast<unsigned int>(std::round(y_float_));
}

//--------------------------------------------------------------------
// Lava implementation
//--------------------------------------------------------------------
LavaClass::LavaClass(MonsterData monster_data) : MonsterClass(monster_data) {
  priority_ = false;
  sprite_mods_ = alatar_classic::kSpriteMultiColor | alatar_classic::kSpriteExpandX;
  frames_per_ms_ = kFramePerMs;
}

void LavaClass::Update(Uint64 counter) {
  MonsterClass::Update(counter);
}

//--------------------------------------------------------------------
// SlidingGate implementation
//--------------------------------------------------------------------
SlidingGateClass::SlidingGateClass(MonsterData monster_data) : MonsterClass(monster_data) {
  priority_ = false;
  y_delta_ = -1.0f;
  sprite_mods_ = alatar_classic::kSpriteMultiColor;
  frames_per_ms_ = kFramePerMs;
}

void SlidingGateClass::Update(Uint64 counter) {
  MonsterClass::Update(counter);

  // Adjust y location
  y_float_ = y_float_ + y_delta_ * elapsed_ms_ * kPixelsPerMs * kSpeedFactor;

  if (y_float_ >= static_cast<float>(y_initial_)) {
    y_float_ = static_cast<float>(y_initial_);
    y_delta_ = -1.0f;
  }

  if (y_float_ <= static_cast<float>(y_initial_ - kMaxYExcursion)) {
    y_float_ = static_cast<float>(y_initial_ - kMaxYExcursion);
    y_delta_ = 1.0f;
  }

  y_ = static_cast<unsigned int>(std::round(y_float_));
}

//--------------------------------------------------------------------
// LavaTroll implementation
//--------------------------------------------------------------------
LavaTrollClass::LavaTrollClass(MonsterData monster_data) : MonsterClass(monster_data) {
  priority_ = false;
  y_delta_ = -1.0f;
  sprite_mods_ = alatar_classic::kSpriteMultiColor;
  frames_per_ms_ = kFramePerMs;
}

void LavaTrollClass::Update(Uint64 counter) {
  MonsterClass::Update(counter);

  // Adjust y location
  y_float_ = y_float_ + y_delta_ * elapsed_ms_ * kPixelsPerMs * kSpeedFactor;

  if (y_float_ >= static_cast<float>(y_initial_)) {
    y_float_ = static_cast<float>(y_initial_);
    y_delta_ = -1.0f;
  }

  if (y_float_ <= static_cast<float>(y_initial_ - kMaxYExcursion)) {
    y_float_ = static_cast<float>(y_initial_ - kMaxYExcursion);
    y_delta_ = 1.0f;
  }

  y_ = static_cast<unsigned int>(std::round(y_float_));
}

}  // namespace alatar
