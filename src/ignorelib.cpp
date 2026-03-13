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
    bool IgnoreFile::Ignored(std::string_view p, FileType f)
    {
        bool ignored = false;

        for (const Pattern& pattern : _patterns)
        {
            if (f == FileType::file && pattern.DirsOnly) continue;

            std::vector<size_t> separators = findSeparators(p);
            separators.push_back(p.size());

            MatchesInfo info {p.substr(0, separators[0]), p, pattern.Re,
                              !pattern.Negated, ignored};

            if (matches(std::move(info))) return ignored;

            if (!pattern.TopLevelOnly)
            {
                for (size_t i {0}; i < separators.size() - 1; ++i)
                {
                    MatchesInfo substrInfo {
                        p.substr(separators[i], separators[i + 1]), p,
                        pattern.Re, !pattern.Negated, ignored};

                    if (matches(std::move(substrInfo))) return ignored;
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
        if (std::regex_match(info.First.begin(), info.First.end(), info.Re))
        {
            // Early return to mimick .gitignore behavior
            if (info.First != info.Full)
            {
                info.Out = info.ToOutput;
                return true;
            }

            info.Out = info.ToOutput;
        }

        if (std::regex_match(info.Full.begin(), info.Full.end(), info.Re))
            info.Out = info.ToOutput;

        return false;
    }
} // namespace Ignorelib
