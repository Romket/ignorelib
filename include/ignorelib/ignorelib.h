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

#include <ignorelib/pattern.h>

#include <re2/re2.h>

#include <filesystem>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

/**
 * @brief Main Ignorelib namespace.
 */
namespace Ignorelib
{
    /**
     * @brief A class representing a .gitignore-style file.
     *
     * Contains methods for loading patterns from files, parsing from strings,
     * matching individual patterns, and listing matched files from a directory.
     */
    class IgnoreFile
    {
    public:
        /**
         * @brief Construct a new IgnoreFile object.
         *
         * Loads patterns from a .gitignore-style file.
         *
         * @param path The path to the file.
         */
        explicit IgnoreFile(const fs::path& path);

        /**
         * @brief Construct a new IgnoreFile object.
         *
         * Loads patterns from a pre-made list of Pattern objects.
         *
         * @param patterns A vector of Pattern objects to load.
         *
         * @sa Pattern
         */
        explicit IgnoreFile(std::vector<Pattern>&& patterns) :
            _patterns {std::move(patterns)}
        {}

        /**
         * @brief Construct a new IgnoreFile object.
         *
         * Loads patterns from a pre-made list of Pattern objects.
         *
         * @param patterns A list of Pattern objects to load.
         *
         * @sa Pattern
         */
        explicit IgnoreFile(std::initializer_list<Pattern>&& patterns) :
            _patterns(std::make_move_iterator(patterns.begin()),
                      std::make_move_iterator(patterns.end()))
        {}

        /**
         * @brief Construct a new IgnoreFile object.
         *
         * Loads patterns from a pre-made list of Pattern objects.
         *
         * @tparam R An arbitrary container of Pattern objects.
         * @param patterns A container of Pattern objects to load.
         *
         * @sa Pattern
         */
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

        /**
         * @brief Construct a new IgnoreFile object.
         *
         * Generates patterns from a list of strings. These strings should
         * follow .gitignore syntax.
         *
         * @param range A list of strings representing lines in an ignore file.
         */
        explicit IgnoreFile(std::initializer_list<std::string_view> range)
        {
            for (const auto& patternStr : range) addPattern(patternStr);
        }

        /**
         * @brief Construct a new IgnoreFile object.
         *
         * Generates patterns from a list of strings. These strings should
         * follow .gitignore syntax.
         *
         * @tparam R An arbitrary container of strings.
         * @param range A list of strings representing lines in an ignore file.
         */
        template<std::ranges::input_range R>
            requires(std::convertible_to<std::ranges::range_value_t<R>,
                                         std::string_view>)
        explicit IgnoreFile(R&& range)
        {
            for (const auto& patternStr : range) addPattern(patternStr);
        }

        /**
         * @brief Default copy construnctor.
         *
         * @param other IgnoreFile object to copy from.
         */
        IgnoreFile(const IgnoreFile& other) = default;

        /**
         * @brief Default move constructor.
         *
         * @param other IgnoreFile object to move from.
         */
        IgnoreFile(IgnoreFile&& other) = default;

        /**
         * @brief Default copy assignment operator.
         *
         * @param other IgnoreFile object to copy from.
         * @return IgnoreFile& The IgnoreFile object copied to.
         */
        IgnoreFile& operator=(const IgnoreFile& other) = default;

        /**
         * @brief Default move assignment operator.
         *
         * @param other IgnoreFile object to move from.
         * @return IgnoreFile& The IgnoreFile object moved to.
         */
        IgnoreFile& operator=(IgnoreFile&& other) = default;

        /**
         * @brief Default IgnoreFile destructor
         */
        ~IgnoreFile() = default;

        /**
         * @brief Get the _patterns object.
         *
         * @return const std::vector<Pattern>& The _patterns object.
         */
        const std::vector<Pattern>& GetPatterns() const { return _patterns; }

    public:
        /**
         * @brief Check if a path is ignored using "Fast" behavior.
         *
         * The "Fast" behavior describes an optimization git uses in checking
         * files against a .gitignore file. If a pattern excludes a file's
         * containing directory, it will exclude the pattern from being negated
         * later.
         *
         * @param path The path to check.
         * @param type The type of the file.
         * @return true The path is ignored.
         * @return false The path is not ignored.
         */
        bool IgnoredFast(const fs::path& path,
                         fs::file_type   type = fs::file_type::regular) const
        { return ignoredUtil(path, type, false); }

        /**
         * @brief Check if a path is ignored using "Full" behavior.
         *
         * The "Full" behavior means that if a containing path is ignored, this
         * will still be searched. This is in contrast to the behavior of git
         * and .gitignore, where if a containing directory is ignored,
         * everything within is ignored regardless of if it is later negated.
         *
         * @param path The path to check.
         * @param type The type of the file.
         * @return true The path is ignored.
         * @return false The path is not ignored.
         */
        bool IgnoredFull(const fs::path& path,
                         fs::file_type   type = fs::file_type::regular) const
        { return ignoredUtil(path, type, true); }

        /**
         * @brief Lists all ignored files in a given directory using "Fast"
         * behavior.
         *
         * The "Fast" behavior describes an optimization git uses in checking
         * files against a .gitignore file. If a directory is ignored, none of
         * the files it contains will be checked.
         *
         * @param dir The directory to scan.
         * @return std::vector<fs::path> A list of ignored files.
         */
        std::vector<fs::path> ListIgnoredFast(const fs::path& dir) const;

        /**
         * @brief Lists all ignored files in a given directory using "Full"
         * behavior.
         *
         * The "Full" behavior means that if a directory is ignored, all files
         * it contains will still be searched. This is in contrast to the
         * behavior of git and .gitignore, where if a containing directory is
         * ignored, everything within is ignored regardless of if it is later
         * negated.
         *
         * @param dir The directory to scan.
         * @return std::vector<fs::path> A list of ignored files.
         */
        std::vector<fs::path> ListIgnoredFull(const fs::path& dir) const;

        /**
         * @brief Lists all included files in a given directory using "Fast"
         * behavior.
         *
         * The "Fast" behavior describes an optimization git uses in checking
         * files against a .gitignore file. If a directory is ignored, none of
         * the files it contains will be checked.
         *
         * @param dir The directory to scan.
         * @return std::vector<fs::path> A list of included files.
         */
        std::vector<fs::path> ListIncludedFast(const fs::path& dir) const;

        /**
         * @brief Lists all included files in a given directory using "Full"
         * behavior.
         *
         * The "Full" behavior means that if a directory is ignored, all files
         * it contains will still be searched. This is in contrast to the
         * behavior of git and .gitignore, where if a containing directory is
         * ignored, everything within is ignored regardless of if it is later
         * negated.
         *
         * @param dir The directory to scan.
         * @return std::vector<fs::path> A list of included files.
         */
        std::vector<fs::path> ListIncludedFull(const fs::path& dir) const;

        /**
         * @brief Lists all ignored files in the current working directory using
         * "Fast" behavior.
         *
         * The "Fast" behavior describes an optimization git uses in checking
         * files against a .gitignore file. If a directory is ignored, none of
         * the files it contains will be checked.
         *
         * @return std::vector<fs::path> A list of ignored files.
         */
        std::vector<fs::path> ListIgnoredFast() const
        { return ListIgnoredFast(fs::current_path()); }

        /**
         * @brief Lists all ignored files in the current working directory using
         * "Full" behavior.
         *
         * The "Full" behavior means that if a directory is ignored, all files
         * it contains will still be searched. This is in contrast to the
         * behavior of git and .gitignore, where if a containing directory is
         * ignored, everything within is ignored regardless of if it is later
         * negated.
         *
         * @return std::vector<fs::path> A list of ignored files.
         */
        std::vector<fs::path> ListIgnoredFull() const
        { return ListIgnoredFull(fs::current_path()); }

        /**
         * @brief Lists all included files in the current working directory
         * using "Fast" behavior.
         *
         * The "Fast" behavior describes an optimization git uses in checking
         * files against a .gitignore file. If a directory is ignored, none of
         * the files it contains will be checked.
         *
         * @return std::vector<fs::path> A list of included files.
         */
        std::vector<fs::path> ListIncludedFast() const
        { return ListIncludedFast(fs::current_path()); }

        /**
         * @brief Lists all included files in the current working directory
         * using "Full" behavior.
         *
         * The "Full" behavior means that if a directory is ignored, all files
         * it contains will still be searched. This is in contrast to the
         * behavior of git and .gitignore, where if a containing directory is
         * ignored, everything within is ignored regardless of if it is later
         * negated.
         *
         * @return std::vector<fs::path> A list of included files.
         */
        std::vector<fs::path> ListIncludedFull() const
        { return ListIncludedFull(fs::current_path()); }

    private:
        struct MatchesInfo
        {
            std::string               First;
            std::string               Full;
            std::shared_ptr<re2::RE2> Re;
            fs::file_type             File;
            bool                      DirsOnly;
        };

        struct Matched
        {
            bool IsMatched      = false;
            bool EarlyReturnMet = false;
        };

        struct SeparatorInfo
        {
            std::vector<size_t> Separators {};
            std::vector<size_t> Found {};
        };

    private:
        void addPattern(std::string_view s);

        bool ignoredUtil(const fs::path& path,
                         fs::file_type   type,
                         bool            isFullMatch) const;

        static std::vector<size_t> findSeparators(std::string_view sv);

        static Matched matches(MatchesInfo&& info);

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

        SeparatorInfo getSeparatorInfo(std::string_view pathStr) const;

        static size_t getLoopInfo(const SeparatorInfo& sepInfo,
                                  const Pattern&       pattern);

    private:
        std::vector<Pattern> _patterns;

        size_t _mostSeparators {0};
    };
} // namespace Ignorelib
