/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITYCORE_REVISION_DATA_H
#define TRINITYCORE_REVISION_DATA_H

// Git commit information
#define TRINITY_GIT_COMMIT_HASH     "71c4a756a273+"
#define TRINITY_GIT_COMMIT_DATE     "2025-12-01 16:54:49 +0000"
#define TRINITY_GIT_COMMIT_BRANCH   R"(copilot/fix-language-menu-update)"

// Build OS information
#define TRINITY_BUILD_HOST_SYSTEM                   R"(Linux)"
#define TRINITY_BUILD_HOST_SYSTEM_VERSION           R"(6.11.0-1018-azure)"
#define TRINITY_BUILD_HOST_DISTRO_NAME              R"(Ubuntu)"
#define TRINITY_BUILD_HOST_DISTRO_VERSION_ID        R"(24.04)"

// Build target information
#define TRINITY_BUILD_PROCESSOR     R"(x86_64)"

// CMake build information
#define TRINITY_BUILD_CMAKE_COMMAND             R"(/usr/local/bin/cmake)"
#define TRINITY_BUILD_CMAKE_VERSION             R"(3.31.6)"
#define TRINITY_BUILD_CMAKE_SOURCE_DIRECTORY    R"(/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk)"
#define TRINITY_BUILD_CMAKE_BUILD_DIRECTORY     R"(/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk/bin)"

// Database updater base information
#define DATABASE_MYSQL_EXECUTABLE   R"(/usr/bin/mysql)"
#define DATABASE_FULL_DATABASE      "TDB_full_world_335.25101_2025_10_21.sql"

// Windows resource defines
#define TRINITY_COMPANYNAME_STR         "TrinityCore Developers"
#define TRINITY_LEGALCOPYRIGHT_STR      "(c)2008-2025 TrinityCore"
#define TRINITY_FILEVERSION             0,0,0
#define TRINITY_FILEVERSION_STR         "71c4a756a273+ 2025-12-01 16:54:49 +0000 (copilot/fix-language-menu-update branch)"
#define TRINITY_PRODUCTVERSION          TRINITY_FILEVERSION
#define TRINITY_PRODUCTVERSION_STR      TRINITY_FILEVERSION_STR

#endif // TRINITYCORE_REVISION_DATA_H
