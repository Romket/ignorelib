/**
 * @file test_patterns.cpp
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Test different patterns
 * @version 1.0 0.1
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

#include <gtest/gtest.h>

#include <ignorelib/ignorelib.h>

namespace fs = std::filesystem;

TEST(test_ignore, blank_line)
{
    Ignorelib::IgnoreFile file {""};

    //* IgnoredFast testing

    EXPECT_FALSE(file.IgnoredFast("test"));
    EXPECT_FALSE(file.IgnoredFast(""));

    //* IgnoredFull testing

    EXPECT_FALSE(file.IgnoredFull("test"));
    EXPECT_FALSE(file.IgnoredFull(""));
}

TEST(test_ignore, comment)
{
    Ignorelib::IgnoreFile file {"#comment", "\\#escaped"};

    //* IgnoredFast testing

    EXPECT_FALSE(file.IgnoredFast("comment"));
    EXPECT_FALSE(file.IgnoredFast("#comment"));

    EXPECT_FALSE(file.IgnoredFast("escaped"));
    EXPECT_TRUE(file.IgnoredFast("#escaped"));

    //* IgnoredFull testing

    EXPECT_FALSE(file.IgnoredFull("comment"));
    EXPECT_FALSE(file.IgnoredFull("#comment"));

    EXPECT_FALSE(file.IgnoredFull("escaped"));
    EXPECT_TRUE(file.IgnoredFull("#escaped"));
}

TEST(test_ignore, negate)
{
    Ignorelib::IgnoreFile file {"pattern", "!negated", "\\!negated", "a",
                                "!a/contained"};

    //* IgnoredFast testing

    EXPECT_TRUE(file.IgnoredFast("pattern"));

    EXPECT_FALSE(file.IgnoredFast("negated"));

    EXPECT_TRUE(file.IgnoredFast("!negated"));

    EXPECT_TRUE(file.IgnoredFast("a/contained"));
    EXPECT_TRUE(file.IgnoredFast("a/other"));

    //* IgnoredFull testing

    EXPECT_TRUE(file.IgnoredFull("pattern"));

    EXPECT_FALSE(file.IgnoredFull("negated"));

    EXPECT_TRUE(file.IgnoredFull("!negated"));

    EXPECT_FALSE(file.IgnoredFull("a/contained"));
    EXPECT_TRUE(file.IgnoredFull("a/other"));
}

TEST(test_ignore, dir_separators)
{
    Ignorelib::IgnoreFile file {"a", "/b", "c/d/"};

    //* IgnoredFast testing

    EXPECT_TRUE(file.IgnoredFast("a"));
    EXPECT_TRUE(file.IgnoredFast("some/path/to/a"));

    EXPECT_TRUE(file.IgnoredFast("b"));
    EXPECT_FALSE(file.IgnoredFast("some/path/to/b"));

    EXPECT_TRUE(file.IgnoredFast("c/d", fs::file_type::directory));
    EXPECT_FALSE(file.IgnoredFast("c/d", fs::file_type::regular));
    EXPECT_FALSE(
        file.IgnoredFast("some/path/to/c/d", fs::file_type::directory));

    //* IgnoredFull testing

    EXPECT_TRUE(file.IgnoredFull("a"));
    EXPECT_TRUE(file.IgnoredFull("some/path/to/a"));

    EXPECT_TRUE(file.IgnoredFull("b"));
    EXPECT_FALSE(file.IgnoredFull("some/path/to/b"));

    EXPECT_TRUE(file.IgnoredFull("c/d", fs::file_type::directory));
    EXPECT_FALSE(file.IgnoredFull("c/d", fs::file_type::regular));
    EXPECT_FALSE(
        file.IgnoredFull("some/path/to/c/d", fs::file_type::directory));
}

TEST(test_ignore, wildcard)
{
    Ignorelib::IgnoreFile file {"a*b", "c?d"};

    //* IgnoredFast testing

    EXPECT_TRUE(file.IgnoredFast("ab"));
    EXPECT_TRUE(file.IgnoredFast("axb"));
    EXPECT_TRUE(file.IgnoredFast("atestb"));
    EXPECT_FALSE(file.IgnoredFast("a/b"));

    EXPECT_TRUE(file.IgnoredFast("cxd"));
    EXPECT_FALSE(file.IgnoredFast("cd"));
    EXPECT_FALSE(file.IgnoredFast("ctestd"));
    EXPECT_FALSE(file.IgnoredFast("c/d"));

    //* IgnoredFull testing

    EXPECT_TRUE(file.IgnoredFull("ab"));
    EXPECT_TRUE(file.IgnoredFull("axb"));
    EXPECT_TRUE(file.IgnoredFull("atestb"));
    EXPECT_FALSE(file.IgnoredFull("a/b"));

    EXPECT_TRUE(file.IgnoredFull("cxd"));
    EXPECT_FALSE(file.IgnoredFull("cd"));
    EXPECT_FALSE(file.IgnoredFull("ctestd"));
    EXPECT_FALSE(file.IgnoredFull("c/d"));
}

TEST(test_ignore, character_class)
{
    Ignorelib::IgnoreFile file {"a[a-z]", "b[0-9]",  "c[A-Z]",
                                "d[123]", "e[^a-z]", "f\\[ab]"};

    //* IgnoredFast testing

    EXPECT_TRUE(file.IgnoredFast("ab"));
    EXPECT_FALSE(file.IgnoredFast("aB"));
    EXPECT_FALSE(file.IgnoredFast("a3"));
    EXPECT_FALSE(file.IgnoredFast("abc"));

    EXPECT_TRUE(file.IgnoredFast("b3"));
    EXPECT_FALSE(file.IgnoredFast("bq"));
    EXPECT_FALSE(file.IgnoredFast("bG"));
    EXPECT_FALSE(file.IgnoredFast("b83"));

    EXPECT_TRUE(file.IgnoredFast("cH"));
    EXPECT_FALSE(file.IgnoredFast("ca"));
    EXPECT_FALSE(file.IgnoredFast("c3"));
    EXPECT_FALSE(file.IgnoredFast("cKH"));

    EXPECT_TRUE(file.IgnoredFast("d1"));
    EXPECT_FALSE(file.IgnoredFast("d5"));
    EXPECT_FALSE(file.IgnoredFast("da"));
    EXPECT_FALSE(file.IgnoredFast("d23"));

    EXPECT_TRUE(file.IgnoredFast("e3"));
    EXPECT_TRUE(file.IgnoredFast("eR"));
    EXPECT_FALSE(file.IgnoredFast("ea"));
    EXPECT_FALSE(file.IgnoredFast("e8G"));

    EXPECT_TRUE(file.IgnoredFast("f[ab]"));
    EXPECT_FALSE(file.IgnoredFast("fa"));

    //* IgnoredFull testing

    EXPECT_TRUE(file.IgnoredFull("ab"));
    EXPECT_FALSE(file.IgnoredFull("aB"));
    EXPECT_FALSE(file.IgnoredFull("a3"));
    EXPECT_FALSE(file.IgnoredFull("abc"));

    EXPECT_TRUE(file.IgnoredFull("b3"));
    EXPECT_FALSE(file.IgnoredFull("bq"));
    EXPECT_FALSE(file.IgnoredFull("bG"));
    EXPECT_FALSE(file.IgnoredFull("b83"));

    EXPECT_TRUE(file.IgnoredFull("cH"));
    EXPECT_FALSE(file.IgnoredFull("ca"));
    EXPECT_FALSE(file.IgnoredFull("c3"));
    EXPECT_FALSE(file.IgnoredFull("cKH"));

    EXPECT_TRUE(file.IgnoredFull("d1"));
    EXPECT_FALSE(file.IgnoredFull("d5"));
    EXPECT_FALSE(file.IgnoredFull("da"));
    EXPECT_FALSE(file.IgnoredFull("d23"));

    EXPECT_TRUE(file.IgnoredFull("e3"));
    EXPECT_TRUE(file.IgnoredFull("eR"));
    EXPECT_FALSE(file.IgnoredFull("ea"));
    EXPECT_FALSE(file.IgnoredFull("e8G"));

    EXPECT_TRUE(file.IgnoredFull("f[ab]"));
    EXPECT_FALSE(file.IgnoredFull("fa"));
}

TEST(test_ignore, escaped)
{
    Ignorelib::IgnoreFile file {"\\a", "\\\\b", "c\\"};

    //* IgnoredFast testing

    EXPECT_TRUE(file.IgnoredFast("a"));
    EXPECT_FALSE(file.IgnoredFast("\\a"));

    EXPECT_FALSE(file.IgnoredFast("b"));
    EXPECT_TRUE(file.IgnoredFast("\\b"));

    EXPECT_FALSE(file.IgnoredFast("c"));
    EXPECT_FALSE(file.IgnoredFast("c\\"));

    //* IgnoredFull testing

    EXPECT_TRUE(file.IgnoredFull("a"));
    EXPECT_FALSE(file.IgnoredFull("\\a"));

    EXPECT_FALSE(file.IgnoredFull("b"));
    EXPECT_TRUE(file.IgnoredFull("\\b"));

    EXPECT_FALSE(file.IgnoredFull("c"));
    EXPECT_FALSE(file.IgnoredFull("c\\"));
}

TEST(test_ignore, double_asterisk)
{
    Ignorelib::IgnoreFile file {"a/**/b", "c/**", "**/d", "**/e/f"};

    //* IgnoredFast testing

    EXPECT_TRUE(file.IgnoredFast("a/b"));
    EXPECT_TRUE(file.IgnoredFast("a/x/b"));
    EXPECT_TRUE(file.IgnoredFast("a/x/y/b"));
    EXPECT_FALSE(file.IgnoredFast("ab"));
    EXPECT_FALSE(file.IgnoredFast("a/xb"));
    EXPECT_FALSE(file.IgnoredFast("a/x/yb"));

    EXPECT_TRUE(file.IgnoredFast("c/something"));
    EXPECT_FALSE(file.IgnoredFast("c1/something"));
    EXPECT_FALSE(file.IgnoredFast("c"));
    EXPECT_FALSE(file.IgnoredFast("c1"));

    EXPECT_TRUE(file.IgnoredFast("d"));
    EXPECT_FALSE(file.IgnoredFast("d1"));
    EXPECT_TRUE(file.IgnoredFast("some/path/to/d"));
    EXPECT_FALSE(file.IgnoredFast("some/path/to/d1"));

    EXPECT_TRUE(file.IgnoredFast("e/f"));
    EXPECT_FALSE(file.IgnoredFast("e/x/f"));
    EXPECT_TRUE(file.IgnoredFast("some/path/to/e/f"));
    EXPECT_FALSE(file.IgnoredFast("some/path/to/e/x/f"));

    //* IgnoredFull testing

    EXPECT_TRUE(file.IgnoredFull("a/b"));
    EXPECT_TRUE(file.IgnoredFull("a/x/b"));
    EXPECT_TRUE(file.IgnoredFull("a/x/y/b"));
    EXPECT_FALSE(file.IgnoredFull("ab"));
    EXPECT_FALSE(file.IgnoredFull("a/xb"));
    EXPECT_FALSE(file.IgnoredFull("a/x/yb"));

    EXPECT_TRUE(file.IgnoredFull("c/something"));
    EXPECT_FALSE(file.IgnoredFull("c1/something"));
    EXPECT_FALSE(file.IgnoredFull("c"));
    EXPECT_FALSE(file.IgnoredFull("c1"));

    EXPECT_TRUE(file.IgnoredFull("d"));
    EXPECT_FALSE(file.IgnoredFull("d1"));
    EXPECT_TRUE(file.IgnoredFull("some/path/to/d"));
    EXPECT_FALSE(file.IgnoredFull("some/path/to/d1"));

    EXPECT_TRUE(file.IgnoredFull("e/f"));
    EXPECT_FALSE(file.IgnoredFull("e/x/f"));
    EXPECT_TRUE(file.IgnoredFull("some/path/to/e/f"));
    EXPECT_FALSE(file.IgnoredFull("some/path/to/e/x/f"));
}

TEST(test_ignore, re_reserved)
{
    Ignorelib::IgnoreFile file {"a.b"};

    //* IgnoredFast testing

    EXPECT_TRUE(file.IgnoredFast("a.b"));

    //* IgnoredFast IgnoredFull

    EXPECT_TRUE(file.IgnoredFull("a.b"));
}
