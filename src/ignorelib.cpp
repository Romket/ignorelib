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

    bool IgnoreFile::IgnoredFast(const fs::path& path, fs::file_type type) const
    {
        if (fs::exists(path)) type = fs::status(path).type();

        bool ignored = false;

        std::string pathStr {path.string()};

        for (const Pattern& pattern : _patterns)
        {
            std::vector<size_t> separators;
            size_t loopTo = getLoopInfo(separators, pattern, pathStr);

            for (size_t i {0}; i < loopTo; ++i)
            {
                MatchesInfo info {
                    pathStr.substr(separators[i],
                                   separators[i + 1 + pattern.SepCount] -
                                       separators[i] - 1),
                    pathStr.substr(separators[i]), pattern.Re, type,
                    pattern.DirsOnly};

                Matched result = matches(std::move(info));

                if (result.IsMatched) ignored = !pattern.Negated;
                if (result.EarlyReturnMet) return ignored;
            }
        }

        return ignored;
    }

    bool IgnoreFile::IgnoredFull(const fs::path& path, fs::file_type type) const
    {
        if (fs::exists(path)) type = fs::status(path).type();

        bool ignored = false;

        std::string pathStr {path.string()};

        for (const Pattern& pattern : _patterns)
        {
            std::vector<size_t> separators;
            size_t loopTo = getLoopInfo(separators, pattern, pathStr);

            for (size_t i {0}; i < loopTo; ++i)
            {
                if (std::regex_match(pathStr.substr(separators[i]),
                                     pattern.Re) &&
                    (type == fs::file_type::directory || !pattern.DirsOnly))
                {
                    ignored = !pattern.Negated;
                }
            }
        }

        return ignored;
    }

    std::vector<fs::path> IgnoreFile::ListIgnoredFast(const fs::path& dir) const
    {
        std::vector<fs::path> ignored {};

        walk(dir,
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

    std::vector<size_t> IgnoreFile::findSeparators(std::string_view sv) const
    {
        std::vector<size_t> separators;
        separators.reserve(sv.size() - 1);

        for (auto [i, c] : std::views::enumerate(sv))
        {
            if (c == '/') separators.push_back(static_cast<size_t>(i + 1));
        }

        return separators;
    }

    IgnoreFile::Matched IgnoreFile::matches(MatchesInfo&& info) const
    {
        Matched result {};

        if (info.First != info.Full &&
            std::regex_match(info.First.begin(), info.First.end(), info.Re))
        {
            result.IsMatched      = true;
            result.EarlyReturnMet = true;

            return result;
        }

        if (std::regex_match(info.Full.begin(), info.Full.end(), info.Re) &&
            (info.File == fs::file_type::directory || !info.DirsOnly))
        {
            result.IsMatched = true;
        }

        return result;
    }

    size_t IgnoreFile::getLoopInfo(std::vector<size_t>& separators,
                                   const Pattern&       pattern,
                                   std::string_view     pathStr) const
    {
        separators.assign(pathStr.size() + pattern.SepCount + 1,
                          pathStr.size() + 1);
        separators[0] = 0;
        std::vector<size_t> found {findSeparators(pathStr)};

        size_t loopTo {1};
        if (!pattern.TopLevelOnly && found.size() > pattern.SepCount)
        {
            std::ranges::copy(found, separators.begin() + 1);
            loopTo = found.size() - pattern.SepCount + 1;
        }

        return loopTo;
    }
} // namespace Ignorelib
