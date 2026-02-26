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
    bool IgnoreFile::Ignored(const std::string& p)
    {
        bool ignored = false;

        for (const std::pair<std::regex, bool>& pattern : _patterns)
        {
            if (std::regex_match(p, pattern.first)) ignored = !pattern.second;
        }

        return ignored;
    }

    bool IgnoreFile::Ignored(const std::string&& p)
    {
        bool ignored = false;

        for (const std::pair<std::regex, bool>& pattern : _patterns)
        {
            size_t i;
            for (i = 0; i < p.length(); ++i)
            {
                // TODO: Add windows directory separator checks
                if (p[i] == '/') break;
            }
            std::string dir = p.substr(0, i);

            if (std::regex_match(dir, pattern.first))
            {
                // Early return to mimick .gitignore behavior
                if (dir != p) return !pattern.second;

                ignored = !pattern.second;
            }

            if (std::regex_match(p, pattern.first)) ignored = !pattern.second;
        }

        return ignored;
    }

    void IgnoreFile::readFile(std::ifstream&& fileHandle)
    {
        if (!fileHandle.is_open())
            throw std::invalid_argument("Failed to open file");

        for (std::string line; std::getline(fileHandle, line);)
        {
            // TODO: remove all whitespace to make these checks better
            if (line.empty() || line[0] == '#') continue;

            const auto& pattern = convToRe(line);
            _patterns.push_back({std::regex(std::move(pattern.first)),
                                 std::move(pattern.second)});
        }
    }

    std::pair<std::string, bool> IgnoreFile::convToRe(std::string_view sv)
    {
        std::string regexStr;

        bool track = sv[0] == '!';

        for (size_t i = track; i < sv.size(); ++i)
        {
            switch (sv[i])
            {
                case '\\':
                    regexStr.push_back(sv[i]);
                    if (i + 1 < sv.size())
                    {
                        regexStr.push_back(sv[i + 1]);
                        ++i;
                    }
                    break;
                case '*':
                    if (i + 1 == sv.size())
                        regexStr += ".*";
                    else
                        regexStr += "[^\\/\\\\]*";
                    break;
                case '.': regexStr += "\\."; break;
                case '/':
                    if (i + 3 < sv.size() && sv.substr(i, 4) == "/**/")
                    {
                        regexStr += "(?:\\/.*\\/|\\/)";
                        i += 3;
                    }
                    else
                        regexStr += "\\/";
                    break;
                case '#': return {std::move(regexStr), std::move(track)};
                default: regexStr.push_back(sv[i]);
            }
        }

        return {std::move(regexStr), std::move(track)};
    }
} // namespace Ignorelib
