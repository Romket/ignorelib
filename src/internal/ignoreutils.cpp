/**
 * @file ignoreutils.cpp
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Internal utility function implementations
 * @version 1.1
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

#include "ignoreutils.h"

#include <cstddef>
#include <re2/re2.h>

namespace ignorelib
{
    std::optional<Pattern> IgnoreUtils::ConvToPattern(std::string_view sv)
    {
        std::string regexStr;

        Pattern p;

        p.Negated = sv.starts_with('!');
        std::string_view negateRemoved = sv.substr(
            static_cast<size_t>(p.Negated));

        bool anyLevel = negateRemoved.starts_with("**/");
        std::string_view start = negateRemoved.substr(
            3 * static_cast<size_t>(anyLevel));

        for (size_t i {0}; i < start.size(); ++i)
        {
            auto el = getNextElement(start, i, p);
            if (el)
                regexStr += el.value();
            else
                return std::nullopt;
        }

        if (anyLevel) p.TopLevelOnly = false;

        p.Re = std::make_shared<re2::RE2>(regexStr);
        return p;
    }

    std::optional<std::string>
        IgnoreUtils::getNextElement(std::string_view start, size_t& i,
                                    Pattern& p)
    {
        switch (start[i])
        {
        case '\\':
            if (i + 1 < start.size())
            {
                // re2 has certain reserved escape codes, check
                // against escapes to see what needs to be escaped.
                if (escapes.find(start[i + 1]) != std::string_view::npos)
                    return std::string {'\\', start[++i]};
                return std::string {start[++i]};
            }
            else
                // option for invalid pattern
                return std::nullopt;
            break;
        case '*':
            if (i + 1 == start.size())
                // any characters if at the end of the pattern
                return ".*";
            else
                // any characters except directory separators
                return R"([^\/\\]*)";
            break;
        case '.':
            return "\\.";
            break;
        case '/':
            if (i + 3 < start.size() && start.substr(i, 4) == "/**/")
            {
                // any number of directories
                return R"((?:\/.*\/|\/))";
                i += 3;
            }
            else if (i + 3 == start.size() && start.substr(i, 3) == "/**")
                // any contained path, same as '/*'
                return "\\/.*";
            else if (i + 1 == start.size())
                // pattern ends with '/', indicates pattern only matches
                // directories
                p.DirsOnly = true;
            else
            {
                p.TopLevelOnly = true;
                if (i > 0)
                {
                    return "\\/";
                    ++p.SepCount;
                }
            }
            break;
        case '?':
            return "[^\\/]";
            break;
        }

        return std::string {start[i]};
    }
} // namespace ignorelib
