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

#include <iostream>

namespace Ignorelib
{
    bool IgnoreFile::Ignored(std::string_view p)
    {
        bool ignored = false;

        for (const Pattern& pattern : _patterns)
        {
            size_t i;
            for (i = 0; i < p.length(); ++i)
                if (p[i] == '/') break;

            std::string dir {p.substr(0, i)};

            if (std::regex_match(dir, pattern.Re))
            {
                // Early return to mimick .gitignore behavior
                if (dir != p) return !pattern.Negated;

                ignored = !pattern.Negated;
            }

            if (std::regex_match(p.begin(), p.end(), pattern.Re))
                ignored = !pattern.Negated;
        }

        return ignored;
    }

    void IgnoreFile::readFile(std::ifstream&& fileHandle)
    {
        std::cout << "asdgasjdg\n";
        if (!fileHandle.is_open())
            throw std::invalid_argument("Failed to open file");

        for (std::string line; std::getline(fileHandle, line);)
        {

            std::cout << line << '\n';
            // TODO: remove all whitespace to make these checks better
            if (line.empty() || line[0] == '#') continue;

            std::cout << line << '\n';

            const auto result = IgnoreUtils::ConvToPattern(line);
            if (result) _patterns.push_back(std::move(*result));
        }
    }
} // namespace Ignorelib
