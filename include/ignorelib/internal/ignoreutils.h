/**
 * @file ignoreutils.h
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Internal ignorelib utility functions
 * @version 0.1
 * @date 2026-03-04
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

#include <ignorelib/pattern.h>

#include <optional>
#include <string_view>

namespace Ignorelib
{
    class IgnoreUtils
    {
    public:
        static std::optional<Pattern> ConvToPattern(std::string_view sv);

#ifdef IGNORELIB_TESTS
        static std::optional<TestPattern>
        ConvToTestPattern(std::string_view sv);
#endif
    };
} // namespace Ignorelib
