/**
 * @file test_load.cpp
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Test file loading
 * @version 0.1
 * @date 2026-02-10
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

#include <gtest/gtest.h>

#define IGNORELIB_TESTS
#include <ignorelib/ignorelib.h>
#undef IGNORELIB_TESTS

#include <fstream>

#include "test_load_file_constants.h"

TEST(test_load, file)
{
    std::ofstream fileToRead {".test_load_file"};
    for (const std::string_view& line : lines) fileToRead << line << '\n';
    fileToRead.close();

    std::ifstream handle {".test_load_file"};
    ASSERT_TRUE(handle.is_open());

    Ignorelib::IgnoreFile file {".test_load_file"};

    ASSERT_EQ(numPatterns, file._patterns.size());

    for (int i = 0; i < numPatterns; ++i)
    {
        const auto& pattern = file.convToRe(lines[i + 2]);

        EXPECT_EQ(patterns[i], pattern.first);
        EXPECT_EQ(patternNegated[i], pattern.second);
    }
}

TEST(test_load, re_list)
{
    std::vector<std::pair<std::regex, bool>> patternMap;
    for (int i = 0; i < numPatterns; ++i)
    {
        patternMap.push_back(
            {std::regex(std::string(patterns[i])), patternNegated[i]});
    }

    Ignorelib::IgnoreFile file {patternMap};

    ASSERT_EQ(numPatterns, file._patterns.size());

    for (int i = 0; i < numPatterns; ++i)
    {
        const auto& pattern = file.convToRe(lines[i + 2]);

        EXPECT_EQ(patterns[i], pattern.first);
        EXPECT_EQ(patternNegated[i], pattern.second);
    }
}
