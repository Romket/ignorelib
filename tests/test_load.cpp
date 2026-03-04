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

#include <ignorelib/ignorelib.h>
#include <ignorelib/internal/ignoreutils.h>

#include <chrono>
#include <fstream>

#include "test_load_file_constants.h"

#include <iostream>

void Loaded(Ignorelib::IgnoreFile&& file)
{
    ASSERT_EQ(numPatterns, file.GetPatterns().size());

    for (int i = 0; i < numPatterns; ++i)
    {
        const auto result =
            Ignorelib::IgnoreUtils::ConvToTestPattern(lines[i + 2]);

        ASSERT_TRUE(result);

        EXPECT_EQ(patterns[i], result->Str);
        EXPECT_EQ(patternNegated[i], result->P.Negated);
    }
}

TEST(test_load, file)
{
    std::ofstream fileToRead {".test_load_file"};
    for (const std::string_view& line : lines) fileToRead << line << '\n';
    fileToRead.close();

    std::cout << "made a thing\n";

    std::this_thread::sleep_for(std::chrono::milliseconds {1000});

    Ignorelib::IgnoreFile file(".test_load_file");

    Loaded(std::move(file));
}

TEST(test_load, pattern_list)
{
    std::vector<Ignorelib::Pattern> patternMap;
    for (int i = 0; i < numPatterns; ++i)
    {
        patternMap.push_back(
            {std::regex(std::string(patterns[i])), patternNegated[i]});
    }

    Ignorelib::IgnoreFile file {std::move(patternMap)};

    Loaded(std::move(file));
}

TEST(test_load, string_list)
{
    Ignorelib::IgnoreFile file {lines};

    Loaded(std::move(file));
}
