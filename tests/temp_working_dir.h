/**
 * @file temp_working_dir.h
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Creates an empty temporary working directory for testing
 * @version 1.1
 * @date 2026-03-18
 *
 * @copyright Copyright (c) 2026 Luke Houston
 *
 * This file is part of ignorelib.  ignorelib is free software:
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as publishedby the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <filesystem>
#include <fstream>
#include <random>
#include <string>

#include "test_scan_patterns.h"

class TempWorkingDir
{
public:
    inline TempWorkingDir() :
        _base {std::filesystem::current_path()},
        _temp {_base / "tmp" / randomDir(16)}
    {
        std::filesystem::create_directories(_temp);

        std::filesystem::current_path(_temp);

        for (const auto& dir : subdirs)
            std::filesystem::create_directories(dir);

        for (const auto& file : scanFiles)
            std::ofstream {std::filesystem::path {file}};
    }

    inline ~TempWorkingDir()
    {
        std::filesystem::current_path(_base);
        std::filesystem::remove_all(_temp);
    }

private:
    inline static std::string randomDir(size_t&& len)
    {
        std::string chars = "0123456789"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "abcdefghijklmnopqrstuvwxyz";

        std::mt19937 gen {std::random_device {}()};
        std::uniform_int_distribution<std::size_t> dist(0, chars.size() - 1);

        std::string str(len, 0);
        std::generate_n(str.begin(), len, [&]() { return chars[dist(gen)]; });

        return str;
    }

    std::filesystem::path _base;
    std::filesystem::path _temp;
};
