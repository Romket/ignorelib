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

#include <iostream>

namespace Ignorelib
{
    std::optional<Pattern> IgnoreUtils::ConvToPattern(std::string_view sv)
    {
        std::string regexStr;

        Pattern p;

        p.Negated                      = sv.starts_with('!');
        std::string_view negateRemoved = sv.substr(p.Negated);

        bool             anyLevel = negateRemoved.starts_with("**/");
        std::string_view start    = negateRemoved.substr(3 * anyLevel);

        for (size_t i {0}; i < start.size(); ++i)
        {
            switch (start[i])
            {
                case '\\':
                    if (i + 1 < start.size())
                    {
                        if (start[i + 1] != '/')
                        {
                            regexStr.push_back(start[i]);
                            ++i;
                            regexStr.push_back(start[i]);
                        }
                    }
                    else
                        return std::nullopt;
                    break;
                case '*':
                    if (i + 1 == start.size())
                        regexStr += ".*";
                    else
                        regexStr += "[^\\/\\\\]*";
                    break;
                case '.': regexStr += "\\."; break;
                case '/':
                    if (i + 3 < start.size() && start.substr(i, 4) == "/**/")
                    {
                        regexStr += "(?:\\/.*\\/|\\/)";
                        i += 3;
                    }
                    else if (i + 3 == start.size() &&
                             start.substr(i, 3) == "/**")
                        regexStr += "\\/.*";
                    else if (i + 1 == start.size())
                        p.DirsOnly = true;
                    else
                    {
                        p.TopLevelOnly = true;
                        if (i > 0)
                        {
                            regexStr += "\\/";
                            ++p.SepCount;
                        }
                    }
                    break;
                case '?': regexStr += "[^\\/]"; break;
                default: regexStr.push_back(start[i]);
            }
        }

        std::cout << regexStr << '\n';

        if (anyLevel) p.TopLevelOnly = false;

        p.Re = std::regex(std::move(regexStr));
        return p;
    }

#ifdef IGNORELIB_TESTS
    std::optional<TestPattern>
    IgnoreUtils::ConvToTestPattern(std::string_view sv)
    {
        std::string regexStr;

        TestPattern p;

        p.P.Negated                    = sv.starts_with('!');
        std::string_view negateRemoved = sv.substr(p.P.Negated);

        bool             anyLevel = negateRemoved.starts_with("**/");
        std::string_view start    = negateRemoved.substr(3 * anyLevel);

        for (size_t i {0}; i < start.size(); ++i)
        {
            switch (start[i])
            {
                case '\\':
                    if (i + 1 < start.size())
                    {
                        if (start[i + 1] != '/')
                        {
                            regexStr.push_back(start[i]);
                            ++i;
                            regexStr.push_back(start[i]);
                        }
                    }
                    else
                        return std::nullopt;
                    break;
                case '*':
                    if (i + 1 == start.size())
                        regexStr += ".*";
                    else
                        regexStr += "[^\\/\\\\]*";
                    break;
                case '.': regexStr += "\\."; break;
                case '/':
                    if (i + 3 < start.size() && start.substr(i, 4) == "/**/")
                    {
                        regexStr += "(?:\\/.*\\/|\\/)";
                        i += 3;
                    }
                    else if (i + 3 == start.size() &&
                             start.substr(i, 3) == "/**")
                        regexStr += "\\/.*";
                    else if (i + 1 == start.size())
                        p.P.DirsOnly = true;
                    else
                    {
                        p.P.TopLevelOnly = true;
                        if (i > 0)
                        {
                            regexStr += "\\/";
                            ++p.P.SepCount;
                        }
                    }
                    break;
                case '?': regexStr += "[^\\/]"; break;
                default: regexStr.push_back(start[i]);
            }
        }

        std::cout << regexStr << '\n';

        if (anyLevel) p.P.TopLevelOnly = false;

        p.P.Re = std::regex(regexStr);
        p.Str  = std::move(regexStr);

        return p;
    }
#endif
} // namespace Ignorelib
