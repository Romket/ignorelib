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
        {
            addPattern(line);
        }
    }

    bool IgnoreFile::Ignored(const fs::path& path,
                             fs::file_type   type,
                             bool            runEarlyReturnLogic) const
    {
        if (fs::exists(path)) type = fs::status(path).type();

        bool ignored = false;

        std::string pathStr {path.string()};

        for (const Pattern& pattern : _patterns)
        {
            std::vector<size_t> separators {};
            if (runEarlyReturnLogic || !pattern.TopLevelOnly)
            {
                separators = findSeparators(pathStr);
                for (size_t i {0}; i <= pattern.SepCount; ++i)
                    separators.push_back(pathStr.size());
            }

            if (runEarlyReturnLogic)
            {
                MatchesInfo info {
                    pathStr.substr(0, separators[pattern.SepCount]), path,
                    pattern.Re, type, pattern.DirsOnly};

                Matched result = matches(std::move(info));

                if (result.IsMatched) ignored = !pattern.Negated;
                if (result.EarlyReturnMet) return ignored;
            }
            else if (std::regex_match(pathStr, pattern.Re) &&
                     (type == fs::file_type::directory || !pattern.DirsOnly))
            {
                return !pattern.Negated;
            }

            if (!pattern.TopLevelOnly)
            {
                for (size_t i {0}; i + pattern.SepCount < separators.size() - 1;
                     ++i)
                {
                    if (runEarlyReturnLogic)
                    {
                        MatchesInfo subdirInfo {
                            pathStr.substr(
                                separators[i] + 1,
                                separators[i + 1 + pattern.SepCount] -
                                    (separators[i] + 1)),
                            pathStr.substr(separators[i] + 1), pattern.Re, type,
                            pattern.DirsOnly};

                        Matched subdirResult = matches(std::move(subdirInfo));

                        if (subdirResult.IsMatched) ignored = !pattern.Negated;
                        if (subdirResult.EarlyReturnMet) return ignored;
                    }
                    else if (std::regex_match(pathStr.substr(separators[i] + 1),
                                              pattern.Re) &&
                             (type == fs::file_type::directory ||
                              !pattern.DirsOnly))
                    {
                        return !pattern.Negated;
                    }
                }
            }
        }

        return ignored;
    }

    std::vector<fs::path> IgnoreFile::ListIgnored(const fs::path& dir) const
    {
        if (!fs::exists(dir) || !fs::is_directory(dir)) return {};

        std::vector<fs::path> ignored {};

        for (auto it = fs::recursive_directory_iterator {dir};
             it != fs::recursive_directory_iterator {}; ++it)
        {
            fs::path path = fs::relative(it->path(), dir);

            fs::file_type type = it->status().type();

            if (Ignored(path, type, false))
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
        }

        return ignored;
    }

    std::vector<fs::path> IgnoreFile::ListIncluded(const fs::path& dir) const
    {
        if (!fs::exists(dir) || !fs::is_directory(dir)) return {};

        std::vector<fs::path> included {};

        for (auto it = fs::recursive_directory_iterator {dir};
             it != fs::recursive_directory_iterator {}; ++it)
        {
            fs::path path = fs::relative(it->path(), dir);

            if (!Ignored(path))
            {
                if (it->status().type() == fs::file_type::regular)
                    included.push_back(path);
            }
        }

        return included;
    }

    std::vector<size_t> IgnoreFile::findSeparators(std::string_view sv) const
    {
        std::vector<size_t> separators;
        separators.reserve(sv.size());

        for (auto [i, c] : std::views::enumerate(sv))
        {
            if (c == '/') separators.push_back(static_cast<size_t>(i));
        }

        return separators;
    }

    IgnoreFile::Matched IgnoreFile::matches(MatchesInfo&& info) const
    {
        Matched result {};

        if (std::regex_match(info.First.begin(), info.First.end(), info.Re) &&
            info.First != info.Full)
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
} // namespace Ignorelib
