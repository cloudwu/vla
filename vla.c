#include "vla.h"

//vla_handle v = vla_create(type, n);
//vla_open(type, name, n);
//vla_close(name);
//vla_open_handle(type, name, v);
//vla_get_handle(name);
//vla_size(name);
//vla_resize(name, sz);
//vla_rele


#include <stdlib.h>

struct vla_container {
	int n;
	int esize;
	unsigned char *buffer;	
};

static struct vla_handle
vla_new_handle(int n, int esize) {
	struct vla_container *c = (struct vla_container *)malloc(sizeof(*c));
	c->n = n;
	c->esize = esize;
	c->buffer = (unsigned char *)malloc(n * esize);
	struct vla_handle ret = { (void *)c };
	return ret;
}

static void
vla_resize_handle(struct vla_handle h, int sz) {
	struct vla_container *c = (struct vla_container *)h.ptr;
	c->n = sz;
	c->buffer = realloc(c->buffer, sz * c->esize);
}

static void
vla_remap_handle(struct vla_accessor_ *acc) {
	struct vla_container *c = (struct vla_container *)acc->h.ptr;
	*acc->p = (void *)c->buffer;
	acc->n = &c->n;
	acc->cap = c->n;
}

void
vla_resize_internal(struct vla_accessor_ *acc, int n, int esize) {
	if (acc->h.ptr == NULL) {
		acc->h = vla_new_handle(n, esize);
	} else {
		int newcap = acc->cap;
		while (newcap < n)
			newcap = newcap * 3 / 2;
		vla_resize_handle(acc->h, newcap);
	}
	vla_remap_handle(acc);
	*acc->n = n;
}

void
vla_release_internal(struct vla_handle h) {
	struct vla_container *c = (struct vla_container *)h.ptr;
	if (c) {
		free(c->buffer);
		free(c);
	}
}
