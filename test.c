#define LUA_LIB

#include "vla.h"
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <assert.h>

static void
init_handle(lua_State *L, vla_handle_t handle, int n) {
	vla_using(tmp, int, handle, L);
	int i;
	for (i=0;i<n;i++) {
		vla_push(tmp, i, L);
	}
}

static void
copy_handle_table(lua_State *L, vla_handle_t handle, int n) {
	vla_using(tmp, int, handle, L);
	vla_resize(tmp, n, L);
	assert(n == vla_size(tmp));
	lua_createtable(L, n, 0);
	int i;
	for (i=0;i<n;i++) {
		lua_pushinteger(L, tmp[i]);
		lua_rawseti(L, -2, i+1);
	}
}

static int
ltestlua(lua_State *L) {
	vla_stack_handle(handle, int);
	init_handle(L, handle, 1000);
	copy_handle_table(L, handle, 10);
	// do not need vla_close(tmp)
	return 1;
}

static int
ltestheap(lua_State *L) {
	vla_stack_handle(handle, int);
	vla_using(tmp, int, handle, NULL);
	vla_resize(tmp, 1000, NULL);
	int i;
	for (i=0;i<vla_size(tmp);i++) {
		tmp[i] = i;
	}
	vla_close(tmp);
	return 0;
}

struct ud {
	vla_handle_t list;
};

static int
lpushc(lua_State *L) {
	struct ud *p = (struct ud *)lua_touserdata(L, 1);
	vla_using(tmp, int, p->list, NULL);
	int v = luaL_checkinteger(L, 2);
	vla_push(tmp, v, NULL);
	return 0;
}

static int
lpushlua(lua_State *L) {
	struct ud *p = (struct ud *)lua_touserdata(L, 1);
	vla_using(tmp, int, p->list, L);
	int v = luaL_checkinteger(L, 2);
	vla_push(tmp, v, L);
	int id;
	if ((id = vla_luaid(tmp, L))) {
		// ref lua vla object
		lua_pushvalue(L, id);
		lua_setiuservalue(L, 1, 1);
	}
	return 0;
}

static int
llen(lua_State *L) {
	struct ud *p = (struct ud *)lua_touserdata(L, 1);
	vla_using(tmp, int, p->list, NULL);
	int sz = vla_size(tmp);
	lua_pushinteger(L, sz);
	return 1;
}

static int
lget(lua_State *L) {
	struct ud *p = (struct ud *)lua_touserdata(L, 1);
	vla_using(tmp, int, p->list, NULL);
	int sz = vla_size(tmp);
	int idx = luaL_checkinteger(L, 2);
	if (idx <= 0 || idx > sz)
		return luaL_error(L, "boundary %d/%d", idx, sz);
	lua_pushinteger(L, tmp[idx-1]);
	return 1;
}

static int
lset(lua_State *L) {
	struct ud *p = (struct ud *)lua_touserdata(L, 1);
	vla_using(tmp, int, p->list, NULL);
	int sz = vla_size(tmp);
	int idx = luaL_checkinteger(L, 2);
	if (idx <= 0 || idx > sz)
		return luaL_error(L, "boundary %d/%d", idx, sz);
	int v = luaL_checkinteger(L, 3);
	tmp[idx-1] = v;
	return 0;
}

static int
lrelease(lua_State *L) {
	struct ud *p = (struct ud *)lua_touserdata(L, 1);
	vla_close_handle(p->list);
	return 0;
}

static int
ltestud(lua_State *L) {
	struct ud * p = NULL;
	if (lua_toboolean(L, 1)) {
		// Use C heap
		p = (struct ud *)lua_newuserdatauv(L, sizeof(*p), 0);
		p->list = vla_new(int, 0, NULL);
		if (luaL_newmetatable(L, "CUD")) {
			luaL_Reg l[] = {
				{ "__call", lpushc },
				{ "__len", llen },
				{ "__index", lget },
				{ "__newindex", lset },
				{ "__gc", lrelease },
				{ NULL, NULL },
			};
			luaL_setfuncs(L, l, 0);
		}
	} else {
		// Use Lua memory
		p = (struct ud *)lua_newuserdatauv(L, sizeof(*p), 1);
		p->list = vla_new(int, 0, L);
		lua_setiuservalue(L, -2, 1);	// ref lua vla object
		if (luaL_newmetatable(L, "LUAUD")) {
			luaL_Reg l[] = {
				{ "__call", lpushlua },
				{ "__len", llen },
				{ "__index", lget },
				{ "__newindex", lset },
				{ NULL, NULL },
			};
			luaL_setfuncs(L, l, 0);
		}
	}
	lua_setmetatable(L, -2);
	return 1;
}

LUAMOD_API int
luaopen_vlatest(lua_State *L) {
	luaL_checkversion(L);
	luaL_Reg l[] = {
		{ "testlua", ltestlua },
		{ "testheap", ltestheap },
		{ "testud", ltestud },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);

	return 1;
}
