/**
 * @file ignorelib.h
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Main header file for ignorelib
 * @version 0.1
 * @date 2026-02-06
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

#include <ignorelib/internal/ignoreutils.h>
#include <ignorelib/pattern.h>

#include <filesystem>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

namespace Ignorelib
{
    class IgnoreFile
    {
    public:
        explicit IgnoreFile(const fs::path& path);

        explicit IgnoreFile(std::vector<Pattern>&& vecPatterns) :
            _patterns {std::move(vecPatterns)}
        {}

        explicit IgnoreFile(std::initializer_list<Pattern>&& patterns) :
            _patterns(std::make_move_iterator(patterns.begin()),
                      std::make_move_iterator(patterns.end()))
        {}

        template<std::ranges::input_range R>
            requires(
                std::convertible_to<std::ranges::range_value_t<R>, Pattern> &&
                !std::same_as<std::remove_cvref_t<R>, IgnoreFile>)
        explicit IgnoreFile(R&& patterns) :
            _patterns {std::move(patterns.begin()), std::move(patterns.end())}
        {}

        explicit IgnoreFile(std::initializer_list<std::string_view> range)
        {
            for (const auto& patternStr : range) addPattern(patternStr);
        }

        template<std::ranges::input_range R>
            requires(std::convertible_to<std::ranges::range_value_t<R>,
                                         std::string_view>)
        explicit IgnoreFile(R&& range)
        {
            for (const auto& patternStr : range) addPattern(patternStr);
        }

        IgnoreFile(const IgnoreFile& other) = default;
        IgnoreFile(IgnoreFile&& other)      = default;

        IgnoreFile& operator=(const IgnoreFile& other) = default;
        IgnoreFile& operator=(IgnoreFile&& other)      = default;

        ~IgnoreFile() = default;

        const std::vector<Pattern>& GetPatterns() const { return _patterns; }

    public:
        bool IgnoredFast(const fs::path& path,
                         fs::file_type   type = fs::file_type::regular) const;
        bool IgnoredFull(const fs::path& path,
                         fs::file_type   type = fs::file_type::regular) const;

        std::vector<fs::path> ListIgnoredFast(const fs::path& dir) const;
        std::vector<fs::path> ListIgnoredFull(const fs::path& dir) const;

        std::vector<fs::path> ListIncludedFast(const fs::path& dir) const;
        std::vector<fs::path> ListIncludedFull(const fs::path& dir) const;

        std::vector<fs::path> ListIgnoredFast() const
        { return ListIgnoredFast(fs::current_path()); }
        std::vector<fs::path> ListIgnoredFull() const
        { return ListIgnoredFull(fs::current_path()); }

        std::vector<fs::path> ListIncludedFast() const
        { return ListIncludedFast(fs::current_path()); }
        std::vector<fs::path> ListIncludedFull() const
        { return ListIncludedFull(fs::current_path()); }

    private:
        struct MatchesInfo
        {
            std::string       First;
            std::string       Full;
            const std::regex& Re;
            fs::file_type     File;
            bool              DirsOnly;
        };

        struct Matched
        {
            bool IsMatched      = false;
            bool EarlyReturnMet = false;
        };

    private:
        void addPattern(std::string_view s)
        {
            if (s.empty() || s.front() == '#') return;

            const auto result = IgnoreUtils::ConvToPattern(s);
            if (result) _patterns.push_back(std::move(*result));
        }

        std::vector<size_t> findSeparators(std::string_view sv) const;

        Matched matches(MatchesInfo&& info) const;

        template<typename Fn>
        void walk(const fs::path& dir, Fn&& f) const
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

        size_t getLoopInfo(std::vector<size_t>& separators,
                           const Pattern&       pattern,
                           std::string_view     pathStr) const;

    private:
        std::vector<Pattern> _patterns;
    };
} // namespace Ignorelib
