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
        std::string line;
        while (fileHandle >> line)
        {
            if (line.empty()) continue;

            bool   escaped = false;
            size_t i       = 0;

            for (i = 0; i < line.size(); ++i)
            {
                if (line[i] == '\\')
                    escaped = !escaped;
                else
                    escaped = false;

                if (!escaped && line[i] == '#') break;
            }

            _patterns.push_back(line.substr(0, std::move(i)));
        }
    }
} // namespace Ignorelib
