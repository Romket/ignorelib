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

#include <re2/re2.h>

#include <filesystem>
#include <fstream>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Ignorelib
{
    enum class FileType
    {
        file,
        directory
    };

    class IgnoreFile
    {
    public:
        explicit inline IgnoreFile(const std::filesystem::path& path)
        { readFile(path); }

        explicit inline IgnoreFile(std::filesystem::path&& path)
        { readFile(std::move(path)); }

        explicit inline IgnoreFile(std::vector<Pattern>&& vecPatterns) :
            _patterns {std::move(vecPatterns)}
        {}

        explicit inline IgnoreFile(std::initializer_list<Pattern>&& patterns) :
            _patterns(std::move(patterns.begin()), std::move(patterns.end()))
        {}

        template<std::ranges::input_range R>
            requires(
                std::convertible_to<std::ranges::range_value_t<R>, Pattern> &&
                !std::same_as<std::remove_cvref_t<R>, IgnoreFile> &&
                !std::convertible_to<std::ranges::range_value_t<R>,
                                     std::string_view>)
        explicit inline IgnoreFile(const R& patterns) :
            _patterns {patterns.begin(), patterns.end()}
        {}

        template<std::ranges::input_range R>
            requires(
                std::convertible_to<std::ranges::range_value_t<R>, Pattern> &&
                !std::same_as<std::remove_cvref_t<R>, IgnoreFile>)
        explicit inline IgnoreFile(R&& patterns) :
            _patterns {std::move(patterns.begin()), std::move(patterns.end())}
        {}

        explicit inline IgnoreFile(
            std::initializer_list<std::string_view>&& range)
        {
            for (std::string_view patternStr : range) addPattern(patternStr);
        }

        template<std::ranges::input_range R>
            requires(std::convertible_to<std::ranges::range_value_t<R>,
                                         std::string_view> &&
                     !std::same_as<std::remove_cvref_t<R>, IgnoreFile>)
        explicit inline IgnoreFile(const R& range)
        {
            for (std::string_view patternStr : range) addPattern(patternStr);
        }

        template<std::ranges::input_range R>
            requires(std::convertible_to<std::ranges::range_value_t<R>,
                                         std::string> &&
                     !std::same_as<std::remove_cvref_t<R>, IgnoreFile>)
        explicit inline IgnoreFile(R&& range)
        {
            for (std::string_view patternStr : range) addPattern(patternStr);
        }

        inline IgnoreFile(const IgnoreFile& other) = default;
        inline IgnoreFile(IgnoreFile&& other)      = default;

        inline IgnoreFile& operator=(const IgnoreFile& other) = default;
        inline IgnoreFile& operator=(IgnoreFile&& other)      = default;

        inline ~IgnoreFile() = default;

        const std::vector<Pattern>& GetPatterns() const { return _patterns; }

    public:
        bool Ignored(std::string_view path,
                     const FileType&  type                = FileType::file,
                     bool             runEarlyReturnLogic = true);

        inline std::vector<std::filesystem::path>
        ListIgnored(const std::filesystem::path& dir)
        { return getIgnoredList(std::filesystem::path {dir}); }
        inline std::vector<std::filesystem::path>
        ListIgnored(std::filesystem::path&& dir)
        { return getIgnoredList(std::move(dir)); }
        inline std::vector<std::filesystem::path> ListIgnored()
        { return getIgnoredList(std::filesystem::current_path()); }

        inline std::vector<std::filesystem::path>
        ListIncluded(const std::filesystem::path& dir)
        { return getIncludedList(std::filesystem::path {dir}); }
        inline std::vector<std::filesystem::path>
        ListIncluded(std::filesystem::path&& dir)
        { return getIncludedList(std::move(dir)); }
        inline std::vector<std::filesystem::path> ListIncluded()
        { return getIncludedList(std::filesystem::current_path()); }

    private:
        struct MatchesInfo
        {
            // cppcheck-suppress unusedStructMember
            std::string_view First;
            // cppcheck-suppress unusedStructMember
            std::string_view          Full;
            std::shared_ptr<re2::RE2> Re;
            // cppcheck-suppress unusedStructMember
            const bool& ToOutput;
            // cppcheck-suppress unusedStructMember
            bool& Out;
            // cppcheck-suppress unusedStructMember
            FileType File;
            // cppcheck-suppress unusedStructMember
            bool DirsOnly;
        };

    private:
        inline void addPattern(std::string_view s)
        {
            if (s.empty() || s.front() == '#') return;

            auto result = IgnoreUtils::ConvToPattern(s);
            if (result) _patterns.push_back(std::move(*result));
        }

        void readFile(std::ifstream&& fileHandle);

        std::vector<size_t> findSeparators(std::string_view sv);

        bool matches(MatchesInfo&& info);

        std::vector<std::filesystem::path>
        getIgnoredList(std::filesystem::path&& dir);
        std::vector<std::filesystem::path>
        getIncludedList(std::filesystem::path&& dir);

    private:
        std::vector<Pattern> _patterns;
    };
} // namespace Ignorelib
