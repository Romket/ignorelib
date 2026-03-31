/**
 * @file test_scan_patterns.h
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Patterns to use to test directory scanning
 * @version 0.1
 * @date 2026-03-19
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

#pragma once

#include <string_view>

// These tests are not very rigorous, they are only designed to make sure the
// directory scanning works correctly. Rigorous pattern matching testing is
// performed in test_ignore.cpp

constexpr int              numScanPatterns = 5;
constexpr std::string_view scanPatterns[numScanPatterns] {"a", "!a/c", "#c",
                                                          "d[0-9]", "**/x/y"};

constexpr std::string_view subdirs[] {"a", "b", "b/x"};

constexpr int              numScanFiles = 6;
constexpr std::string_view scanFiles[numScanFiles] {"a/b", "a/c", "b/x/y",
                                                    "#c",  "d1",  "da"};

constexpr int              numScanIgnored = 4;
constexpr std::string_view scanIgnoredExpected[numScanIgnored] {"a/b", "a/c",
                                                                "b/x/y", "d1"};

constexpr int              numScanIncluded = numScanFiles - numScanIgnored;
constexpr std::string_view scanIncludedExpected[numScanIncluded] {"#c", "da"};
