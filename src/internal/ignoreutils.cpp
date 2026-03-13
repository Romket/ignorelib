/**
 * @file ignoreutils.cpp
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Internal utility function implementations
 * @version 0.1
 * @date 2026-03-04
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

#include <ignorelib/internal/ignoreutils.h>

namespace Ignorelib
{
    std::optional<Pattern> IgnoreUtils::ConvToPattern(std::string_view sv)
    {
        std::string regexStr;

        Pattern p;

        p.Negated = sv[0] == '!';

        for (size_t i = p.Negated; i < sv.size(); ++i)
        {
            switch (sv[i])
            {
                case '\\':
                    regexStr.push_back(sv[i]);
                    if (i + 1 < sv.size())
                    {
                        if (sv[i + 1] != '/')
                        {
                            regexStr.push_back(sv[i]);
                            ++i;
                            regexStr.push_back(sv[i]);
                        }
                    }
                    else
                        return std::nullopt;
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
                    else if (i + 1 == sv.size())
                        p.DirsOnly = true;
                    else
                    {
                        p.TopLevelOnly = true;
                        regexStr += "\\/";
                    }
                    break;
                case '?': regexStr += "[^\\/]"; break;
                default: regexStr.push_back(sv[i]);
            }
        }

        p.Re = std::regex(std::move(regexStr));
        return p;
    }

#ifdef IGNORELIB_TESTS
    std::optional<TestPattern>
    IgnoreUtils::ConvToTestPattern(std::string_view sv)
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
                    else
                        return std::nullopt;
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
                case '?': regexStr += "[^\\/]"; break;
                case '#':
                    return TestPattern {
                        Pattern {std::regex {regexStr}, std::move(track)},
                        regexStr};
                default: regexStr.push_back(sv[i]);
            }
        }

        return TestPattern {Pattern {std::regex {regexStr}, std::move(track)},
                            regexStr};
    }
#endif
} // namespace Ignorelib
