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
    bool IgnoreFile::Ignored(std::string_view path,
                             const FileType&  type,
                             bool             runEarlyReturnLogic)
    {
        bool ignored = false;

        for (const Pattern& pattern : _patterns)
        {
            std::vector<size_t> separators {};
            if (runEarlyReturnLogic || !pattern.TopLevelOnly)
            {
                separators = findSeparators(path);
                for (size_t i {0}; i <= pattern.SepCount; ++i)
                    separators.push_back(path.size());
            }

            if (runEarlyReturnLogic)
            {
                MatchesInfo info {path.substr(0, separators[pattern.SepCount]),
                                  path,
                                  pattern.Re,
                                  !pattern.Negated,
                                  ignored,
                                  type,
                                  pattern.DirsOnly};

                if (matches(std::move(info))) return ignored;
            }
            else if (re2::RE2::FullMatch(path, *pattern.Re) &&
                     (type == FileType::directory || !pattern.DirsOnly))
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
                        MatchesInfo substrInfo {
                            path.substr(separators[i] + 1,
                                        separators[i + 1 + pattern.SepCount] -
                                            (separators[i] + 1)),
                            path.substr(separators[i] + 1),
                            pattern.Re,
                            !pattern.Negated,
                            ignored,
                            type,
                            pattern.DirsOnly};

                        if (matches(std::move(substrInfo))) return ignored;
                    }
                    else if (re2::RE2::FullMatch(path.substr(separators[i] + 1),
                                                 *pattern.Re) &&
                             (type == FileType::directory || !pattern.DirsOnly))
                    {
                        return !pattern.Negated;
                    }
                }
            }
        }

        return ignored;
    }

    void IgnoreFile::readFile(std::ifstream&& fileHandle)
    {
        if (!fileHandle.is_open())
            throw std::invalid_argument("Failed to open file");

        for (std::string line; std::getline(fileHandle, line);)
        {
            addPattern(line);
        }
    }

    std::vector<size_t> IgnoreFile::findSeparators(std::string_view sv)
    {
        std::vector<size_t> separators {};

        for (auto [i, c] : std::views::enumerate(sv))
        {
            if (c == '/')
                separators.push_back(std::move(static_cast<size_t>(i)));
        }

        return separators;
    }

    bool IgnoreFile::matches(MatchesInfo&& info)
    {
        if (re2::RE2::FullMatch(info.First, *info.Re) &&
            info.First != info.Full)
        {
            info.Out = info.ToOutput;
            return true;
        }

        if (re2::RE2::FullMatch(info.Full, *info.Re) &&
            (info.File == FileType::directory || !info.DirsOnly))
        {
            info.Out = info.ToOutput;
        }

        return false;
    }

    std::vector<std::filesystem::path>
    IgnoreFile::getIgnoredList(std::filesystem::path&& dir)
    {

        if (!std::filesystem::exists(dir) ||
            !std::filesystem::is_directory(dir))
            return {};

        std::vector<std::filesystem::path> ignored {};

        for (auto it =
                 std::filesystem::recursive_directory_iterator {std::move(dir)};
             it != std::filesystem::recursive_directory_iterator {}; ++it)
        {
            std::filesystem::path path =
                std::filesystem::relative(it->path(), dir);

            FileType type = std::filesystem::is_directory(path) ?
                                FileType::directory :
                                FileType::file;

            if (Ignored(path.string(), type, false))
            {
                if (type == FileType::directory)
                {
                    it.disable_recursion_pending();
                    for (const auto& entry :
                         std::filesystem::recursive_directory_iterator {path})
                    {
                        if (!std::filesystem::is_directory(entry))
                            ignored.push_back(std::move(entry.path()));
                    }
                }
                else
                    ignored.push_back(std::move(path));
            }
        }

        return ignored;
    }

    std::vector<std::filesystem::path>
    IgnoreFile::getIncludedList(std::filesystem::path&& dir)
    {
        if (!std::filesystem::exists(dir) ||
            !std::filesystem::is_directory(dir))
            return {};

        std::vector<std::filesystem::path> included {};

        for (auto it =
                 std::filesystem::recursive_directory_iterator {std::move(dir)};
             it != std::filesystem::recursive_directory_iterator {}; ++it)
        {
            std::filesystem::path path =
                std::filesystem::relative(it->path(), dir);

            FileType type = std::filesystem::is_directory(path) ?
                                FileType::directory :
                                FileType::file;

            if (!Ignored(path.string(), type))
            {
                if (type == FileType::file) included.push_back(std::move(path));
            }
        }

        return included;
    }
} // namespace Ignorelib
