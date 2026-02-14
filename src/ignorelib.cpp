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
    void IgnoreFile::readFile(std::ifstream&& fileHandle)
    {
        if (!fileHandle.is_open())
            throw std::invalid_argument("Failed to open file");

        std::string line;
        while (fileHandle >> line)
        {
            // TODO: remove all whitespace to make these checks better
            if (line.empty() || line[0] == '#') continue;

            _patterns.push_back(convToRe(line));
        }
    }

    std::regex IgnoreFile::convToRe(std::string_view sv)
    {
        std::string regexStr;

        for (size_t i = 0; i < sv.size(); ++i)
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
                    if (i + 1 < sv.size() && sv[i + 1] == '*')
                    {
                        regexStr += "?*";
                        ++i;
                    }
                    else
                        regexStr += "[!/\\\\]*";
                    break;
                case '.': regexStr += "\\."; break;
                case '#': return std::regex(std::move(regexStr));
                default: regexStr.push_back(sv[i]);
            }
        }

        return std::regex(std::move(regexStr));
    }
} // namespace Ignorelib
