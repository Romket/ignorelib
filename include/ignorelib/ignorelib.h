/**
 * @file ignorelib.h
 * @author Luke Houston (Romket) (lukehouston08@gmail.com)
 * @brief Main header file for ignorelib
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

#pragma once

#include <filesystem>

namespace Ignorelib
{
    class IgnoreFile
    {
    public:
        explicit inline IgnoreFile(const std::filesystem::path& path) noexcept :
            _filePath(path)
        {
            loadFile();
        }

        explicit inline IgnoreFile(const std::filesystem::path&& path) noexcept
            : _filePath(std::move(path))
        {
            loadFile();
        }

        inline IgnoreFile(IgnoreFile& other) noexcept { swap(*this, other); }

        inline IgnoreFile(IgnoreFile&& other) noexcept
        {
            swap(*this, std::move(other));
        }

        inline IgnoreFile& operator=(const std::filesystem::path& path)
        {
            _filePath = path;
            return *this;
        }

        inline IgnoreFile& operator=(const std::filesystem::path&& path)
        {
            _filePath = std::move(path);
            return *this;
        }

        inline IgnoreFile& operator=(IgnoreFile& other) noexcept
        {
            swap(*this, other);
            return *this;
        }

        inline IgnoreFile& operator=(IgnoreFile&& other) noexcept
        {
            swap(*this, std::move(other));
            return *this;
        }

        inline friend void swap(IgnoreFile& first, IgnoreFile& second) noexcept
        {
            std::swap(first._filePath, second._filePath);
        }

        inline friend void swap(IgnoreFile& first, IgnoreFile&& second) noexcept
        {
            std::swap(first._filePath, second._filePath);
        }

        inline ~IgnoreFile() = default;

    private:
        void loadFile();

        std::filesystem::path _filePath;
    };
} // namespace Ignorelib
