/**
 * @file test_patterns.cpp
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Test different patterns
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

#include <gtest/gtest.h>

#include <ignorelib/ignorelib.h>

TEST(test_ignore, blank_line)
{
    Ignorelib::IgnoreFile file {""};

    EXPECT_FALSE(file.Ignored("test"));
    EXPECT_FALSE(file.Ignored(""));
}

TEST(test_ignore, comment)
{
    Ignorelib::IgnoreFile file {"#comment", "\\#escaped"};

    EXPECT_FALSE(file.Ignored("comment"));
    EXPECT_FALSE(file.Ignored("#comment"));

    EXPECT_FALSE(file.Ignored("escaped"));
    EXPECT_TRUE(file.Ignored("#escaped"));
}

TEST(test_ignore, negate)
{
    Ignorelib::IgnoreFile file {"pattern", "!negated", "\\!negated", "a",
                                "!a/contained"};

    EXPECT_TRUE(file.Ignored("pattern"));

    EXPECT_FALSE(file.Ignored("negated"));

    EXPECT_TRUE(file.Ignored("!negated"));

    EXPECT_TRUE(file.Ignored("a/contained"));
}

// TODO: directory separators in .gitignore are weird

TEST(test_ignore, wildcard)
{
    Ignorelib::IgnoreFile file {"a*b", "c?d"};

    EXPECT_TRUE(file.Ignored("ab"));
    EXPECT_TRUE(file.Ignored("axb"));
    EXPECT_TRUE(file.Ignored("atestb"));
    EXPECT_FALSE(file.Ignored("a/b"));

    EXPECT_TRUE(file.Ignored("cxd"));
    EXPECT_FALSE(file.Ignored("cd"));
    EXPECT_FALSE(file.Ignored("ctestd"));
    EXPECT_FALSE(file.Ignored("c/d"));
}

TEST(test_ignore, character_class)
{
    Ignorelib::IgnoreFile file {"a[a-z]", "b[0-9]",  "c[A-Z]",
                                "d[123]", "e[^a-z]", "f\\[ab]"};

    EXPECT_TRUE(file.Ignored("ab"));
    EXPECT_FALSE(file.Ignored("aB"));
    EXPECT_FALSE(file.Ignored("a3"));
    EXPECT_FALSE(file.Ignored("abc"));

    EXPECT_TRUE(file.Ignored("b3"));
    EXPECT_FALSE(file.Ignored("bq"));
    EXPECT_FALSE(file.Ignored("bG"));
    EXPECT_FALSE(file.Ignored("b83"));

    EXPECT_TRUE(file.Ignored("cH"));
    EXPECT_FALSE(file.Ignored("ca"));
    EXPECT_FALSE(file.Ignored("c3"));
    EXPECT_FALSE(file.Ignored("cKH"));

    EXPECT_TRUE(file.Ignored("d1"));
    EXPECT_FALSE(file.Ignored("d5"));
    EXPECT_FALSE(file.Ignored("da"));
    EXPECT_FALSE(file.Ignored("d23"));

    EXPECT_TRUE(file.Ignored("e3"));
    EXPECT_TRUE(file.Ignored("eR"));
    EXPECT_FALSE(file.Ignored("ea"));
    EXPECT_FALSE(file.Ignored("e8G"));

    EXPECT_TRUE(file.Ignored("f[ab]"));
    EXPECT_FALSE(file.Ignored("fa"));
}

TEST(test_ignore, escaped)
{
    Ignorelib::IgnoreFile file {"\\a", "\\\\b", "c\\"};

    EXPECT_TRUE(file.Ignored("a"));
    EXPECT_FALSE(file.Ignored("\\a"));

    EXPECT_FALSE(file.Ignored("b"));
    EXPECT_TRUE(file.Ignored("\\b"));

    EXPECT_FALSE(file.Ignored("c"));
    EXPECT_FALSE(file.Ignored("c\\"));
}

// TODO: add better support for leading '**/' and trailing '/**'
TEST(test_ignore, double_asterisk)
{
    Ignorelib::IgnoreFile file {"a/**/b"};

    EXPECT_TRUE(file.Ignored("a/b"));
    EXPECT_TRUE(file.Ignored("a/x/b"));
    EXPECT_TRUE(file.Ignored("a/x/y/b"));
    EXPECT_FALSE(file.Ignored("ab"));
    EXPECT_FALSE(file.Ignored("a/xb"));
    EXPECT_FALSE(file.Ignored("a/x/yb"));
}

TEST(test_ignore, re_reserved)
{
    Ignorelib::IgnoreFile file {"a.b"};

    EXPECT_TRUE(file.Ignored("a.b"));
}
