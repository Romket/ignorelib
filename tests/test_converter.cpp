/**
 * @file test_converter.cpp
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Test converting pattern strings to regex
 * @version 0.1
 * @date 2026-03-05
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

#include <ignorelib/internal/ignoreutils.h>
#include <ignorelib/pattern.h>

#include "test_load_file_constants.h"

TEST(test_convert, convert)
{
    for (int i {2}; i < numLines; ++i)
    {
        auto result = Ignorelib::IgnoreUtils::ConvToTestPattern(lines[i]);

        ASSERT_TRUE(result);

        EXPECT_EQ(patterns[i - 2], result->Str);
        EXPECT_EQ(patternNegated[i - 2], result->P.Negated);
    }
}
