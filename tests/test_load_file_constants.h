/**
 * @file test_load_file_constants.h
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Constant values for testing file loading
 * @version 0.1
 * @date 2026-02-24
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

#include <string_view>

namespace Ignorelib
{
    constexpr int              numLines        = 11;
    constexpr std::string_view lines[numLines] = {
        "# ignored",       "",
        "ignore",          "!negate",
        "charclass[0-9]",  "file.extension",
        "path/to",         "pattern# comment",
        "dir/*",           "dir/*.ext",
        "dir/**/otherFile"};

    constexpr int              numPatterns           = 9;
    constexpr std::string_view patterns[numPatterns] = {
        "ignore",
        "negate",
        "charclass[0-9]",
        "file\\.extension",
        "path\\/to",
        "pattern",
        "dir\\/.*",
        "dir\\/[^\\/\\\\]*\\.ext",
        "dir(?:\\/.*\\/|\\/)otherFile"};

    constexpr bool patternNegated[numPatterns] = {
        false, true, false, false, false, false, false, false, false,
    };
}; // namespace Ignorelib
