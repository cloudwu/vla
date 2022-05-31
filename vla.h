#ifndef VARIANT_LENGTH_ARRAY_H
#define VARIANT_LENGTH_ARRAY_H

#include <stddef.h>

#define VLA_STACK_SIZE 240

struct vla_handle { void * ptr; };

struct vla_stack_ {
	struct vla_handle *ref;
	int n;
	unsigned char buffer[VLA_STACK_SIZE];	
};

struct vla_accessor_ {
	void **p;
	int *n;
	int cap;
	struct vla_handle h;
};

#define vla_open(name, type, n) \
	type *name; \
	struct vla_stack_ name##_stack_; \
	struct vla_accessor_ name##_accessor_; \
	vla_init_stack_((void **)&name, &name##_stack_, &name##_accessor_, n, sizeof(type))

#define vla_size(name) (*name##_accessor_.n)

#define vla_resize(name, sz) vla_resize_(&name##_accessor_, sz, sizeof(*name))

void vla_resize_internal(struct vla_accessor_ *acc, int sz, int esize);

static inline void
vla_resize_(struct vla_accessor_ *acc, int sz, int esize) {
	if (sz <= acc->cap)
		*acc->n = sz;
	else
		vla_resize_internal(acc, sz, esize);
}

static inline void
vla_init_stack_(void **v, struct vla_stack_ *s, struct vla_accessor_ *acc, int n, int esize) {
	acc->p = v;
	acc->n = &s->n;
	acc->cap = (VLA_STACK_SIZE + esize - 1)/ esize;
	acc->h.ptr = NULL;
	s->ref = &acc->h;
	s->n = 0;
	*v = s->buffer;
	vla_resize_(acc, n, esize);
}

#define vla_close(name) vla_close_(&name##_stack_)

void vla_release_internal(struct vla_handle h);

static inline void
vla_close_(struct vla_stack_ *s) {
	if (s->ref->ptr == NULL)
		return;
	vla_release_internal(*s->ref);
}

#endif