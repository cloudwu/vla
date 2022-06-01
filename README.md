Variant Length Array for C
====================

`vla_handle_t` : A handle of VLA object , it can be passed into C functions or be saved in the C structs.

```C
// Create a VLA object with name and type on C stack.
void vla_stack_handle(name, type);

// Create a VLA object with type in Lua memory. (Or C heap when L == NULL)
vla_handle_t vla_new(type, n, L);

// Create an accessor (type * name) for VLA object (handle).
// You can use the accessor to access the data of VLA, it's just a raw pointer of the data.
void vla_using(name, type, handle, L);

// Sync accessor if VLA object changes
void vla_sync(name);

// Close VLA object via accessor
void vla_close(name);

// Close VLA handle
void vla_close_handle(handle);

// Resize VLA object via accessor
void vla_resize(name, n, L);

// Get length of VLA object via accessor
int vla_size(name);

// Push v into VLA object via accessor
void vla_push(name, v, L);

// Get Lua stack id via accessor, 0 when unchanged. If you need refer this VLA object, you should save the lua object at stack id.
int vla_luaid(name, L);
```

Example 1:

```C
#include "vla.h"
#include <assert.h>

static void
test(vla_handle_t handle) {
	vla_using(p, int, handle, NULL);
	for (i=0;i<1000;i++) {
		assert(p[i] == i);
	}
	// Do not close p
}

static void
init(vla_handle_t handle) {
	vla_using(p, int, handle, NULL);
	for (i=0;i<1000;i++) {
		vla_push(p, i);
	}
}

int
main() {
	vla_stack_handle(handle, int);	// Create an int array on stack
	vla_using(p, int, handle, NULL);	// Create an acessor int *p for data accessing.

	init(handle);

	vla_sync(p);	// init() may changes array

	assert(vla_size(p) == 1000);

	test(handle);


	vla_close(p);	// close before return

	return 0;
}

```

Example 2:

```C
#include <lua.h>
#include <lauxlib.h>

static int
lfoobar(lua_State *L) {
	int n = luaL_checkinteger(L, 1);
	vla_stack_handle(handle, int);
	vla_using(p, int, handle, L);	// Create an acessor int *p
	vla_resize(p, n);	// Resize handle to n
	int i;
	for (i=0;i<n;i++) {
		p[i] = i;
	}
	// Do not need vla_close(p) because VLA memory will be collect by LUA GC.
	return 0;
}

```
