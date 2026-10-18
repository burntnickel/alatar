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

#ifndef H_ALATAR_UTIL
#define H_ALATAR_UTIL

#include <source_location>
#include <string>
#include <string_view>
#include <vector>

namespace BurningLogic {

int SignExtendNibble(unsigned char c);

void LeftStringTrim(std::string& s);
void RightStringTrim(std::string& s);
void StringTrim(std::string& s);
std::string LeftStringTrimCopy(std::string s);
std::string RightStringTrimCopy(std::string s);
std::string StringTrimCopy(std::string s);

void StringToUpper(std::string& s);
void StringToLower(std::string& s);
std::string StringToUpperCopy(std::string_view sv);
std::string StringToLowerCopy(std::string_view sv);

std::vector<std::string> split(const std::string& string);
std::vector<std::string> split(const std::string& string, char delim);

double throwing_stod(const std::string& str, std::source_location loc = std::source_location::current());
int throwing_stoi(const std::string& str, std::source_location loc = std::source_location::current());

template <typename T>
  requires std::integral<T>
bool is_odd(T x) {
  return (x & 1) != 0;
}

}  // namespace BurningLogic

#endif
