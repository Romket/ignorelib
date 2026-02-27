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

#include <filesystem>
#include <fstream>
#include <ranges>
#include <regex>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gtest/gtest_prod.h>

#ifdef IGNORELIB_TESTS
#define private public
#endif

namespace Ignorelib
{
    class IgnoreFile
    {
    public:
        explicit inline IgnoreFile(const std::filesystem::path& path) noexcept
        {
            loadFile(path);
        }

        explicit inline IgnoreFile(std::filesystem::path&& path) noexcept
        {
            loadFile(std::move(path));
        }

        explicit inline IgnoreFile(
            const std::span<std::pair<std::regex, bool>>& patterns) noexcept :
            _patterns {patterns.begin(), patterns.end()}
        {}

        explicit inline IgnoreFile(
            std::vector<std::pair<std::regex, bool>>&& patterns) noexcept :
            _patterns {std::move(patterns)}
        {}

        explicit IgnoreFile(std::initializer_list<std::string_view>&& range)
        {
            for (const std::string_view& p : range)
            {
                if (p.empty() || p[0] == '#') continue;

                const auto pattern = convToRe(p);
                _patterns.push_back({std::regex(std::move(pattern.first)),
                                     std::move(pattern.second)});
            }
        }

        template<std::ranges::input_range R>
            requires(std::convertible_to<std::ranges::range_value_t<R>,
                                         std::string_view> &&
                     !std::same_as<std::remove_cvref_t<R>, IgnoreFile>)
        explicit inline IgnoreFile(const R& range)
        {
            for (const std::string_view& p : range)
            {
                if (p.empty() || p[0] == '#') continue;

                const auto pattern = convToRe(p);
                _patterns.push_back({std::regex(std::move(pattern.first)),
                                     std::move(pattern.second)});
            }
        }

        template<std::ranges::input_range R>
            requires(std::convertible_to<std::ranges::range_value_t<R>,
                                         std::string> &&
                     !std::same_as<std::remove_cvref_t<R>, IgnoreFile>)
        explicit inline IgnoreFile(R&& range)
        {
            for (std::string&& p : range)
            {
                if (p.empty() || p[0] == '#') continue;

                const auto pattern = convToRe(p);
                _patterns.push_back({std::regex(std::move(pattern.first)),
                                     std::move(pattern.second)});
            }
        }

        inline IgnoreFile(IgnoreFile& other) noexcept { swap(*this, other); }

        inline IgnoreFile(IgnoreFile&& other) noexcept
        {
            swap(*this, std::move(other));
        }

        inline IgnoreFile& operator=(const std::filesystem::path& path)
        {
            loadFile(path);
            return *this;
        }

        inline IgnoreFile& operator=(std::filesystem::path&& path)
        {
            loadFile(std::move(path));
            return *this;
        }

        inline IgnoreFile& operator=(IgnoreFile& other) noexcept
        {
            swap(*this, other);
            return *this;
        }

        inline IgnoreFile& operator=(IgnoreFile&& other) noexcept
        {
            swap(*this, std::move(other));
            return *this;
        }

        inline ~IgnoreFile() = default;

    public:
        bool Ignored(const std::string& p);
        bool Ignored(std::string&& p);

    private:
        inline friend void swap(IgnoreFile& first, IgnoreFile& second) noexcept
        {
            std::swap(first._patterns, second._patterns);
        }

        inline friend void swap(IgnoreFile& first, IgnoreFile&& second) noexcept
        {
            std::swap(first._patterns, second._patterns);
        }

        inline void loadFile(const std::filesystem::path& path)
        {
            readFile(path);
        }
        inline void loadFile(std::filesystem::path&& path)
        {
            readFile(std::move(path));
        }

        void readFile(std::ifstream&& fileHandle);

        std::pair<std::string, bool> convToRe(std::string_view sv);

    private:
        std::vector<std::pair<std::regex, bool>> _patterns;
    };
} // namespace Ignorelib
