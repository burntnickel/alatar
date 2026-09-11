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

#ifndef H_ALATAR_VALUE_CYCLE
#define H_ALATAR_VALUE_CYCLE

#include <algorithm>
#include <string>
#include <span>
#include <vector>

namespace alatar {

template <class T>
class ValueCycle {
 public:
  explicit ValueCycle(std::span<const T> sequence);
  T GetValue(void) const;
  ValueCycle& operator++();
  ValueCycle operator++(int);
  operator T() const;

 private:
  std::vector<T> data_;
  std::size_t index_ = 0;
};

}  // namespace alatar

// Implementation follows

// --------------------------------------------------------------------------------
// Anonymous namespace so these are not visible to the linker outside of this file
namespace {

const std::string empty_sequence_text = "sequence must be non-empty";

}  // namespace

namespace alatar {

template <class T>
ValueCycle<T>::ValueCycle(std::span<const T> sequence) {
  if (sequence.empty()) {
    throw std::invalid_argument(empty_sequence_text);
  }

  data_.resize(sequence.size());
  std::copy(sequence.begin(), sequence.end(), data_.begin());
}

template <class T>
T ValueCycle<T>::GetValue(void) const {
  return data_[index_];
}

template <class T>
ValueCycle<T>& ValueCycle<T>::operator++() {
  index_ = index_ + 1;

  if (index_ >= data_.size()) {
    index_ = 0;
  }

  return *this;
}

template <class T>
ValueCycle<T> ValueCycle<T>::operator++(int) {
  ValueCycle tmp = *this;
  ++*this;

  return tmp;
}

template <class T>
ValueCycle<T>::operator T() const {
  return data_[index_];
}

}  // namespace alatar

#endif
