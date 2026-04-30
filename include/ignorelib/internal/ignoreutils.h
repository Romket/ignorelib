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

#include <ignorelib/ignorelib.h>
#include <ignorelib/pattern.h>

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

namespace Ignorelib
{
    class IgnoreUtils
    {
    private:
        friend class IgnoreFile;

#ifdef IGNORELIB_TESTS
    public:
#else
    private:
#endif
        static std::optional<Pattern> convToPattern(std::string_view sv);

    private:
        static std::vector<size_t> findSeparators(std::string_view sv);

        static IgnoreFile::Matched matches(IgnoreFile::MatchesInfo&& info);

        template<typename Fn>
        static void walk(const fs::path& dir, Fn&& f)
        {
            if (!fs::is_directory(dir)) return;

            for (fs::recursive_directory_iterator it {dir};
                 it != fs::recursive_directory_iterator {}; ++it)
            {
                fs::path      path {fs::relative(it->path(), dir)};
                fs::file_type type {it->status().type()};

                if constexpr (std::invocable<Fn&, const fs::path&,
                                             const fs::file_type&>)
                    f(path, type);
                else
                    f(it, path, type);
            }
        }

    private:
        // cppcheck-suppress unusedStructMember
        constexpr static std::string_view _escapes = "\\!#?*[ ";
    };
} // namespace Ignorelib
