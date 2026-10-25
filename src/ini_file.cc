// Copyright 2026 Jude Giampaolo
//
// This file is part of Alatar.
//
// Alien Geometries is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Alien Geometries is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Alien
// Geometries. If not, see <https://www.gnu.org/licenses/>.

#include "ini_file.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "util.h"

namespace BurningLogic {

struct KeyValuePair {
  std::string key;
  std::string value;
};

// Returns the section name in the passed in string, if it is not a valid
// section name it throws
static std::string GetSectionName(std::string& s) {
  const auto len = s.length();

  if (len < 3) {
    throw std::runtime_error("String too short to be a section name");
  }

  if ((s[0] != '[') || (s[len - 1] != ']')) {
    throw std::runtime_error("Section name must begin with [ and end with ]");
  }

  // Make sure there are no spaces or tabs
  if ((s.find(' ') != std::string::npos) || (s.find('\t') != std::string::npos)) {
    throw std::runtime_error("Section name cannot contain whitespace");
  }

  // Make sure section is converted to upper case so it is case insensitive
  return StringToUpperCopy(s.substr(1, len - 2));
}

// Returns the key and value pair for a provided entry, throws if it is not
// a valid key / value specification
static KeyValuePair GetIniKeyValuePair(std::string& s) {
  const auto split_location = s.find('=');

  if (split_location == std::string::npos) {
    throw std::runtime_error("No = found in key/value pair");
  }

  // Make sure key is converted to upper case so it is case insensitive
  const std::string key = StringToUpperCopy(StringTrimCopy(s.substr(0, split_location - 1)));
  const std::string value = StringTrimCopy(s.substr(split_location + 1));

  if (key.length() == 0) {
    throw std::runtime_error("No key found before =");
  }

  if (value.length() == 0) {
    throw std::runtime_error("No value found after =");
  }

  // Make sure there are no spaces or tabs in the key
  if ((key.find(' ') != std::string::npos) || (key.find('\t') != std::string::npos)) {
    throw std::runtime_error("Key cannot contain whitespace");
  }

  return {key, value};
}

IniFile::IniFile(std::istream& instream) {
  bool done_reading = false;
  std::string cur_section = "";

  while (!done_reading) {
    std::string next_line;

    std::getline(instream, next_line);

    if (!instream) {
      done_reading = true;
    } else {
      // Skip blank lines
      if (next_line.length() > 0) {
        std::string tmp_string = StringTrimCopy(next_line);

        // Skip comment lines
        if ((tmp_string[0] != ';') && (tmp_string[0] != '#')) {
          if (tmp_string[0] == '[') {
            cur_section = GetSectionName(tmp_string);
            std::cout << "Section: [" << cur_section << "]\n";
          } else {
            auto key_value_pair = GetIniKeyValuePair(tmp_string);
            std::cout << "    " << key_value_pair.key << " = " << key_value_pair.value << "\n";

            auto num_keys = sections_.count(cur_section);

            if (num_keys == 0) {
              IniFileSection tmp_config_section_map;
              sections_.insert({cur_section, tmp_config_section_map});
            }

            sections_[cur_section].insert({key_value_pair.key, key_value_pair.value});
          }
        }
      }
    }
  }
}

std::optional<std::string> IniFile::GetAsString(std::string_view section, std::string_view key) const {
  ErrorCondition ec;

  const auto ret = GetAsString(section, key, ec);

  // As of now this function never throws as we can either always return as string or as not found
  if (IsErrorCondition(ec)) {
    throw std::runtime_error("Cannot convert value to string: " + ret.value_or(""));
  }

  return ret;
}

std::optional<std::string> IniFile::GetAsString(std::string_view section, std::string_view key,
                                                ErrorCondition& ec) const {
  auto upper_section = StringToUpperCopy(section);
  auto upper_key = StringToUpperCopy(key);

  const auto num_sections = sections_.count(upper_section);

  if (num_sections == 0) {
    ec = NO_SECTION;
    return std::nullopt;
  }

  const auto tmp_section = sections_.at(upper_section);
  const auto num_keys = tmp_section.count(upper_key);

  if (num_keys == 0) {
    ec = NO_KEY;
    return std::nullopt;
  }

  ec = NO_ERROR;
  return tmp_section.at(upper_key);
}

std::optional<int> IniFile::GetAsInt(std::string_view section, std::string_view key) const {
  int ret_int;
  ErrorCondition ec;

  auto as_string = GetAsString(section, key, ec);

  if (ec != NO_ERROR) {
    return std::nullopt;
  }

  // Throws if the value cannot be converted
  ret_int = throwing_stoi(as_string.value());

  return ret_int;
}

std::optional<int> IniFile::GetAsInt(std::string_view section, std::string_view key,
                                     ErrorCondition& ec) const {
  std::optional<int> tmp_ret;

  auto as_string = GetAsString(section, key, ec);

  if (ec != NO_ERROR) {
    return std::nullopt;
  }

  try {
    tmp_ret = GetAsInt(section, key);
  }

  catch ([[maybe_unused]] const std::invalid_argument& e) {
    ec = ERR_INVALID_ARGUMENT;
    return std::nullopt;
  }

  catch ([[maybe_unused]] const std::out_of_range& e) {
    ec = ERR_OUT_OF_RANGE;
    return std::nullopt;
  }

  return tmp_ret;
}

std::optional<unsigned int> IniFile::GetAsUnsigned(std::string_view section, std::string_view key) const {
  auto as_int = GetAsInt(section, key);

  if (as_int.has_value()) {
    if (as_int.value() < 0) {
      throw std::runtime_error("Unsigned value less than zero");
    }

    return static_cast<unsigned int>(as_int.value());
  }

  return std::nullopt;
}

std::optional<unsigned int> IniFile::GetAsUnsigned(std::string_view section, std::string_view key,
                                                   ErrorCondition& ec) const {
  auto as_int = GetAsInt(section, key, ec);

  if (as_int.has_value()) {
    if (as_int.value() < 0) {
      ec = ERR_OUT_OF_RANGE;
      return std::nullopt;
    }

    return static_cast<unsigned int>(as_int.value());
  }

  return std::nullopt;
}

std::optional<double> IniFile::GetAsDouble(std::string_view section, std::string_view key) const {
  double ret_double;
  ErrorCondition ec;

  auto as_string = GetAsString(section, key, ec);

  if (ec != NO_ERROR) {
    return std::nullopt;
  }

  // Throws if the value cannot be converted
  ret_double = throwing_stod(as_string.value());

  return ret_double;
}

std::optional<double> IniFile::GetAsDouble(std::string_view section, std::string_view key,
                                           ErrorCondition& ec) const {
  std::optional<double> tmp_ret;

  auto as_string = GetAsString(section, key, ec);

  if (ec != NO_ERROR) {
    return std::nullopt;
  }

  try {
    tmp_ret = GetAsDouble(section, key);
  }

  catch ([[maybe_unused]] const std::invalid_argument& e) {
    ec = ERR_INVALID_ARGUMENT;
    return std::nullopt;
  }

  catch ([[maybe_unused]] const std::out_of_range& e) {
    ec = ERR_OUT_OF_RANGE;
    return std::nullopt;
  }

  return tmp_ret;
}

bool IniFile::IsErrorCondition(ErrorCondition ec) {
  return (ec != NO_ERROR) && (ec != NO_SECTION) && (ec != NO_KEY);
}

IniFile LoadIniFile(std::filesystem::path path) {
  std::ifstream in(path);

  if (!in.is_open()) {
    // std::cerr << "Error: " << std::strerror(errno) << "(" << path << ")" << std::endl;
    throw std::runtime_error("Failed to open: " + path.string() + " : " + std::strerror(errno));
  }

  IniFile tmp(in);

  in.close();

  return tmp;
}

}  // namespace BurningLogic
