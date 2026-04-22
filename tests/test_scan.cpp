/**
 * @file test_scan.cpp
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Tests for directory scanning functionality
 * @version 0.1
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

#include <ignorelib/ignorelib.h>

#include <gtest/gtest.h>

#include "temp_working_dir.h"
#include "test_scan_patterns.h"

TEST(test_scan, list_ignored)
{
    Ignorelib::IgnoreFile file {scanPatterns};

    TempWorkingDir temp;

    auto ignoredFast = file.ListIgnoredFast();
    auto ingoredFull = file.ListIgnoredFull();

    EXPECT_EQ(ignoredFast.size(), numFastScanIgnored);
    EXPECT_EQ(ingoredFull.size(), numFullScanIgnored);

    for (const auto& path : ignoredFast)
    {
        bool found = false;
        for (const auto& expected : fastScanIgnoredExpected)
        {
            if (path.string() == expected) found = true;
        }

        EXPECT_TRUE(found);
    }

    for (const auto& path : ingoredFull)
    {
        bool found = false;
        for (const auto& expected : fullScanIgnoredExpected)
        {
            if (path.string() == expected) found = true;
        }

        EXPECT_TRUE(found);
    }
}

TEST(test_scan, list_included)
{
    Ignorelib::IgnoreFile file {scanPatterns};

    TempWorkingDir temp;

    auto includedFast = file.ListIncludedFast();
    auto includedFull = file.ListIncludedFull();

    EXPECT_EQ(includedFast.size(), numFastScanIncluded);
    EXPECT_EQ(includedFull.size(), numFullScanIncluded);

    for (const auto& path : includedFast)
    {
        bool found = false;
        for (const auto& expected : fastScanIncludedExpected)
        {
            if (path.string() == expected) found = true;
        }

        EXPECT_TRUE(found);
    }

    for (const auto& path : includedFull)
    {
        bool found = false;
        for (const auto& expected : fullScanIncludedExpected)
        {
            if (path.string() == expected) found = true;
        }

        EXPECT_TRUE(found);
    }
}
