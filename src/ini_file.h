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

#ifndef H_BURNINGLOGIC_INI_FILE
#define H_BURNINGLOGIC_INI_FILE

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace BurningLogic {
  
class IniFile {
  using IniFileSection = std::unordered_map<std::string, std::string>;

 public:
  enum ErrorCondition {
    NO_ERROR,
    NO_SECTION,
    NO_KEY,
    ERR_INVALID_ARGUMENT,
    ERR_OUT_OF_RANGE,
    ERR_INVALID_ENTRY
  };

 public:
  IniFile(void) = default;
  IniFile(std::istream& instream);

  std::optional<std::string> GetAsString(std::string_view section, std::string_view key) const;
  std::optional<std::string> GetAsString(std::string_view section, std::string_view key,
                                         ErrorCondition& ec) const;

  std::optional<int> GetAsInt(std::string_view section, std::string_view key) const;
  std::optional<int> GetAsInt(std::string_view section, std::string_view key, ErrorCondition& ec) const;

  std::optional<unsigned int> GetAsUnsigned(std::string_view section, std::string_view key) const;
  std::optional<unsigned int> GetAsUnsigned(std::string_view section, std::string_view key,
                                            ErrorCondition& ec) const;

  std::optional<double> GetAsDouble(std::string_view section, std::string_view key) const;
  std::optional<double> GetAsDouble(std::string_view section, std::string_view key,
                                    ErrorCondition& ec) const;

 private:
  static bool IsErrorCondition(ErrorCondition ec);

 private:
  std::unordered_map<std::string, IniFileSection> sections_ = {};
};

IniFile LoadIniFile(std::filesystem::path path);

}  // namespace BurningLogic

#endif
