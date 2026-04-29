/**
 * @file ignorelib.cpp
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Main source file
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

#include <ignorelib/ignorelib.h>

#include <algorithm>
#include <fstream>

namespace Ignorelib
{
    IgnoreFile::IgnoreFile(const fs::path& path)
    {
        std::ifstream fileHandle {path};

        if (!fileHandle.is_open())
            throw std::invalid_argument("Failed to open file");

        for (std::string line; std::getline(fileHandle, line);)
            addPattern(line);
    }

    std::vector<fs::path> IgnoreFile::ListIgnoredFast(const fs::path& dir) const
    {
        std::vector<fs::path> ignored {};

        walk(dir,
             // loops over contained paths if directory is matched
             [&](auto& it, const fs::path& path, const fs::file_type& type) {
                 if (IgnoredFull(path))
                 {
                     if (type == fs::file_type::directory)
                     {
                         it.disable_recursion_pending();
                         for (const auto& entry :
                              fs::recursive_directory_iterator {path})
                         {
                             if (!fs::is_directory(entry))
                                 ignored.push_back(entry.path());
                         }
                     }
                     else
                         ignored.push_back(path);
                 }
             });

        return ignored;
    }

    std::vector<fs::path> IgnoreFile::ListIgnoredFull(const fs::path& dir) const
    {
        std::vector<fs::path> ignored {};

        walk(dir, [&](const fs::path& path, const fs::file_type& type) {
            if (IgnoredFull(path) && type == fs::file_type::regular)
                ignored.push_back(path);
        });

        return ignored;
    }

    std::vector<fs::path>
    IgnoreFile::ListIncludedFast(const fs::path& dir) const
    {
        std::vector<fs::path> included {};

        walk(dir,
             // excludes looping over matching containing directories
             [&](auto& it, const fs::path& path, const fs::file_type& type) {
                 if (IgnoredFull(path) && type == fs::file_type::directory)
                 {
                     it.disable_recursion_pending();
                     return;
                 }

                 if (!IgnoredFull(path) &&
                     it->status().type() == fs::file_type::regular)
                     included.push_back(path);
             });

        return included;
    }

    std::vector<fs::path>
    IgnoreFile::ListIncludedFull(const fs::path& dir) const
    {
        std::vector<fs::path> ignored {};

        walk(dir, [&](const fs::path& path, const fs::file_type& type) {
            if (!IgnoredFull(path) && type == fs::file_type::regular)
                ignored.push_back(path);
        });

        return ignored;
    }

    bool IgnoreFile::ignoredUtil(const fs::path& path,
                                 fs::file_type   type,
                                 bool            isFullMatch) const
    {
        if (fs::exists(path)) type = fs::status(path).type();

        bool ignored = false;

        const std::string& pathStr {path.string()};
        SeparatorInfo      sepInfo = getSeparatorInfo(pathStr);

        for (const Pattern& pattern : _patterns)
        {
            size_t loopTo = getLoopInfo(sepInfo, pattern);

            for (size_t i {0}; i < loopTo; ++i)
            {
                MatchesInfo info {
                    pathStr.substr(
                        sepInfo.Separators[i],
                        sepInfo.Separators[i + 1 + pattern.SepCount] -
                            sepInfo.Separators[i] - 1),
                    pathStr.substr(sepInfo.Separators[i]), pattern.Re, type,
                    pattern.DirsOnly};

                Matched result = matches(std::move(info));

                if (result.IsMatched) ignored = !pattern.Negated;
                if (!isFullMatch && result.EarlyReturnMet) return ignored;
            }
        }

        return ignored;
    }

    std::vector<size_t> IgnoreFile::findSeparators(std::string_view sv)
    {
        std::vector<size_t> separators;
        separators.reserve(sv.size() >= 1 ? sv.size() - 1 : 0);

        for (auto [i, c] : std::views::enumerate(sv))
        {
            if (c == '/') separators.push_back(static_cast<size_t>(i + 1));
        }

        return separators;
    }

    IgnoreFile::Matched IgnoreFile::matches(MatchesInfo&& info)
    {
        Matched result {};

        if (info.First != info.Full &&
            re2::RE2::FullMatch(info.First, *info.Re))
        {
            result.IsMatched      = true;
            result.EarlyReturnMet = true;

            return result;
        }

        if (re2::RE2::FullMatch(info.Full, *info.Re) &&
            (info.File == fs::file_type::directory || !info.DirsOnly))
        {
            result.IsMatched = true;
        }

        return result;
    }

    IgnoreFile::SeparatorInfo
    IgnoreFile::getSeparatorInfo(std::string_view pathStr) const
    {
        SeparatorInfo sepInfo {
            .Separators = std::vector<size_t>(
                pathStr.size() + _mostSeparators + 1, pathStr.size() + 1)};

        sepInfo.Separators[0] = 0;
        sepInfo.Found         = findSeparators(pathStr);

        std::ranges::copy(sepInfo.Found, sepInfo.Separators.begin() + 1);

        return sepInfo;
    }

    size_t IgnoreFile::getLoopInfo(const SeparatorInfo& sepInfo,
                                   const Pattern&       pattern)
    {
        if (!pattern.TopLevelOnly && sepInfo.Found.size() > pattern.SepCount)
        {
            return sepInfo.Found.size() - pattern.SepCount + 1;
        }
        return 1;
    }
} // namespace Ignorelib
