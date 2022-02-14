// Some lua sources include this file and some do not,
// so here is the file to be used with all lua sources as some libraries expect it to exist.

#pragma once

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
