/**
 * @file pattern.h
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Pattern struct definition
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

#include <regex>

#ifdef IGNORELIB_TESTS
#include <string>
#endif

namespace Ignorelib
{
    /**
     * @brief Struct representing a single ignore pattern.
     */
    struct Pattern
    {
        /** @brief A regex string for pattern matching. */
        std::regex Re;

        /** @brief Is pattern negated (starts with `!`)? */
        bool Negated {false};

        /**
         * @brief Does pattern only apply to top level paths?
         *
         * (`/` at beginning or middle, cancelled by `**` `/` at the start of
         * the pattern)?
         */
        bool TopLevelOnly {false};

        /** @brief Does pattern only apply to directories (ends with `/`)? */
        bool DirsOnly {false};

        /**
         * @brief The number of directory separators in a pattern.
         *
         * This is excluding leading or trailing separators.
         */
        size_t SepCount {0};
    };

#ifdef IGNORELIB_TESTS
    struct TestPattern
    {
        Pattern     P;
        std::string Str;
    };
#endif
} // namespace Ignorelib
