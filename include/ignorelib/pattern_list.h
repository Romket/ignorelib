/**
 * @file pattern_list.h
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Class for managing pattern lists
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

#pragma once

#include <ignorelib/file.h>

#include <string_view>
#include <vector>

namespace Ignorelib
{
    class PatternList
    {
    public:
        explicit PatternList(std::string_view patternStr);

        explicit PatternList(const IgnoreFile& patternFile);
        explicit PatternList(const IgnoreFile&& patternFile);

        explicit PatternList(const std::vector<std::string>& patternList);
        explicit PatternList(const std::vector<std::string>&& patternList);

        inline PatternList(PatternList& other) noexcept { swap(*this, other); }

        inline PatternList(PatternList&& other) noexcept
        {
            swap(*this, std::move(other));
        }

        inline PatternList& operator=(PatternList& other) noexcept
        {
            swap(*this, other);
            return *this;
        }

        inline PatternList& operator=(PatternList&& other) noexcept
        {
            swap(*this, std::move(other));
            return *this;
        }

    private:
        inline friend void swap(PatternList& first,
                                PatternList& second) noexcept
        {
            std::swap(first._rules, second._rules);
        }

        inline friend void swap(PatternList&  first,
                                PatternList&& second) noexcept
        {
            std::swap(first._rules, second._rules);
        }

        std::vector<std::string> _rules;
    };
} // namespace Ignorelib
