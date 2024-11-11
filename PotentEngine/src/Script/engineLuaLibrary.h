#pragma once
#ifndef _POTENT_ENGINE_LUA_LIBRARY_
#define _POTENT_ENGINE_LUA_LIBRARY_

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "../Scene/engineAllComponents.h"

namespace potent {
	struct LuaVector {
		double x;
		double y;
		double z;
		double w;
	};


	static int libMakeVector(lua_State* L) {
		LuaVector *vector = static_cast<LuaVector*>(lua_newuserdata(L, sizeof(LuaVector)));

		vector->x = 0.0;
		vector->y = 0.0;
		vector->z = 0.0;
		vector->w = 0.0;

		return 1;
	}

	static int addVector(lua_State* L) {
		return 1;
	}

	static const luaL_Reg ENGINE_LUA_LIBRARY[] = {
		{"makeVector", libMakeVector},
		{nullptr, nullptr}
	};

	int openEngineLuaLibrary(lua_State* L) {
		luaL_newlib(L, ENGINE_LUA_LIBRARY);

		return 1;
	}
}

#endif