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

#include "util.h"

#include <algorithm>
#include <locale>

namespace BurningLogic {

int SignExtendNibble(unsigned char c) {
  c = c & 0x0f;

  if (c <= 7) {
    return static_cast<int>(c);
  }

  return -1 * (16 - static_cast<int>(c));
}

//--------------------------------------------------------------------------------
// These function are used to trim white space from strings
//--------------------------------------------------------------------------------

// Based on: https://stackoverflow.com/a/217605/12011195

// Trim from start (in place)
void LeftStringTrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

// Trim from end (in place)
void RightStringTrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
          s.end());
}

// Trim from both ends (in place)
void StringTrim(std::string& s) {
  RightStringTrim(s);
  LeftStringTrim(s);
}

// Trim from start (copying)
std::string LeftStringTrimCopy(std::string s) {
  LeftStringTrim(s);
  return s;
}

// Trim from end (copying)
std::string RightStringTrimCopy(std::string s) {
  RightStringTrim(s);
  return s;
}

// Trim from both ends (copying)
std::string StringTrimCopy(std::string s) {
  StringTrim(s);
  return s;
}

//--------------------------------------------------------------------------------
// These function convert strings to upper / lower case
//--------------------------------------------------------------------------------

// Based on: https://stackoverflow.com/a/24226055/12011195

void StringToUpper(std::string& s) {
  std::locale locale;
  auto to_upper = [&locale](char ch) { return std::use_facet<std::ctype<char>>(locale).toupper(ch); };
  std::transform(s.begin(), s.end(), s.begin(), to_upper);
}

void StringToLower(std::string& s) {
  std::locale locale;
  auto to_lower = [&locale](char ch) { return std::use_facet<std::ctype<char>>(locale).tolower(ch); };
  std::transform(s.begin(), s.end(), s.begin(), to_lower);
}

std::string StringToUpperCopy(std::string_view sv) {
  std::string s(sv);

  StringToUpper(s);
  return s;
}

std::string StringToLowerCopy(std::string_view sv) {
  std::string s(sv);

  StringToLower(s);
  return s;
}

//--------------------------------------------------------------------------------
// "safe" versions that throw when a converison fails
//--------------------------------------------------------------------------------

double throwing_stod(const std::string& str, std::source_location loc) {
  std::size_t charaters_converted;

  double val = std::stod(str, &charaters_converted);

  if (str.length() != charaters_converted) {
    std::string err_msg("Conversion failed: ");
    throw std::invalid_argument(err_msg + loc.file_name() + ":" + std::to_string(loc.line()) + " '" + str +
                                "'");
  }

  return val;
}

int throwing_stoi(const std::string& str, std::source_location loc) {
  std::size_t charaters_converted;

  int val = std::stoi(str, &charaters_converted);

  if (str.length() != charaters_converted) {
    std::string err_msg("Conversion failed: ");
    throw std::invalid_argument(err_msg + loc.file_name() + ":" + std::to_string(loc.line()) + " '" + str +
                                "'");
  }

  return val;
}

}  // namespace BurningLogic

