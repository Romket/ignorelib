/**
 * @file test_load.cpp
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Test file loading
 * @version 1.1
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

#include <re2/re2.h>

#include <array>
#include <fstream>
#include <memory>
#include <vector>

#include "test_load_file_constants.h"

TEST(test_load, file)
{
    std::ofstream fileToRead {".test_load_file"};
    for (const std::string_view& line : lines) fileToRead << line << '\n';
    fileToRead.close();

    ignorelib::IgnoreFile file(".test_load_file");

    EXPECT_EQ(numPatterns, file.GetPatterns().size());
}

TEST(test_load, pattern_list)
{
    // Test std::vector&& constructor
    std::vector<ignorelib::Pattern> patternMap;
    for (size_t i = 0; i < numPatterns; ++i)
    {
        patternMap.push_back(ignorelib::Pattern {
            .Re = std::make_shared<re2::RE2>(patterns[i]),
            .Negated = patternNegated[i]
        });
    }

    ignorelib::IgnoreFile vecFile {patterns};
    EXPECT_EQ(numPatterns, vecFile.GetPatterns().size());

    // Test template range constructors
    std::array<ignorelib::Pattern, numPatterns> patternArray;
    for (size_t i {0}; i < numPatterns; ++i)
    {
        patternArray.at(i) = {
            ignorelib::Pattern {
                                .Re = std::make_shared<re2::RE2>(patterns[i]),
                                .Negated = patternNegated[i]
            }
        };
    }

    ignorelib::IgnoreFile arrFile {patternArray};
    EXPECT_EQ(numPatterns, arrFile.GetPatterns().size());

    ignorelib::IgnoreFile arrMoveFile {std::move(patternArray)};
    EXPECT_EQ(numPatterns, arrMoveFile.GetPatterns().size());

    // Test initializer list constructor
    ignorelib::IgnoreFile listFile {
        {.Re = std::make_shared<re2::RE2>(patterns[0]),
         .Negated = patternNegated[0]}
    };
    EXPECT_EQ(1, listFile.GetPatterns().size());
}

TEST(test_load, string_list)
{
    // Test template range constructors
    std::array<std::string, numLines> linesArray;
    for (size_t i {0}; i < numLines; ++i) linesArray[i] = lines[i];

    ignorelib::IgnoreFile file {linesArray};
    EXPECT_EQ(numPatterns, file.GetPatterns().size());

    ignorelib::IgnoreFile moveFile {std::move(linesArray)};
    EXPECT_EQ(numPatterns, moveFile.GetPatterns().size());

    // Test initializer list constructor
    ignorelib::IgnoreFile listFile {"line1", "line2"};
    EXPECT_EQ(2, listFile.GetPatterns().size());
}

TEST(test_load, other)
{
    ignorelib::IgnoreFile otherConstruct {lines};

    // Test copy constructor
    ignorelib::IgnoreFile constructCopied {otherConstruct};
    EXPECT_EQ(numPatterns, constructCopied.GetPatterns().size());

    // Test move constructor
    ignorelib::IgnoreFile constructMoved {std::move(otherConstruct)};
    EXPECT_EQ(numPatterns, constructMoved.GetPatterns().size());

    ignorelib::IgnoreFile otherAssign {lines};

    // Test copy assignment operator
    ignorelib::IgnoreFile assignCopied {""};
    assignCopied = otherAssign;
    EXPECT_EQ(numPatterns, assignCopied.GetPatterns().size());

    // Test move assignment operator
    ignorelib::IgnoreFile moveCopied {""};
    moveCopied = std::move(otherAssign);
    EXPECT_EQ(numPatterns, moveCopied.GetPatterns().size());
}
