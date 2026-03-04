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
#include <ignorelib/internal/pattern.h>

#include <filesystem>
#include <fstream>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Ignorelib
{
    class IgnoreFile
    {
    public:
        explicit inline IgnoreFile(const std::filesystem::path& path)
        {
            readFile(path);
        }

        explicit inline IgnoreFile(std::filesystem::path&& path)
        {
            readFile(std::move(path));
        }

        explicit inline IgnoreFile(const std::span<Pattern>& patterns) :
            _patterns {patterns.begin(), patterns.end()}
        {
        }

        explicit inline IgnoreFile(std::vector<Pattern>&& patterns) :
            _patterns {std::move(patterns)}
        {
        }

        // TODO: this for loop shows up a lot, make into function
        explicit IgnoreFile(std::initializer_list<std::string_view>&& range)
        {
            for (const std::string_view& p : range)
            {
                if (p.empty() || p[0] == '#') continue;

                const auto result = IgnoreUtils::ConvToPattern(p);
                if (result) _patterns.push_back(std::move(*result));
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

                const auto result = IgnoreUtils::ConvToPattern(p);
                if (result) _patterns.push_back(std::move(*result));
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

                const auto result = IgnoreUtils::ConvToPattern(p);
                if (result) _patterns.push_back(std::move(*result));
            }
        }

        inline IgnoreFile(IgnoreFile& other) { swap(*this, other); }

        inline IgnoreFile(IgnoreFile&& other) { swap(*this, std::move(other)); }

        inline IgnoreFile& operator=(const std::filesystem::path& path)
        {
            readFile(path);
            return *this;
        }

        inline IgnoreFile& operator=(std::filesystem::path&& path)
        {
            readFile(std::move(path));
            return *this;
        }

        inline IgnoreFile& operator=(IgnoreFile& other)
        {
            swap(*this, other);
            return *this;
        }

        inline IgnoreFile& operator=(IgnoreFile&& other)
        {
            swap(*this, std::move(other));
            return *this;
        }

        inline ~IgnoreFile() = default;

        const std::vector<Pattern>& GetPatterns() const { return _patterns; }

    public:
        bool Ignored(std::string_view p);

    private:
        inline friend void swap(IgnoreFile& first, IgnoreFile& second)
        {
            std::swap(first._patterns, second._patterns);
        }

        inline friend void swap(IgnoreFile& first, IgnoreFile&& second)
        {
            std::swap(first._patterns, second._patterns);
        }

        void readFile(std::ifstream&& fileHandle);

    private:
        std::vector<Pattern> _patterns;
    };
} // namespace Ignorelib
