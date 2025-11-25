#include "types.h"
#include "defs.h"
#include "cbuffer.h"


void cbuffer_init(struct cbuffer * cb,
	const unsigned int size,
  const unsigned int esize,
  void * store)
{
	cb->size = size;
	cb->elemsize = esize;
	cb->start = 0;
	cb->count = 0;
	cb->data = store;
}


int cbuffer_is_full(const struct cbuffer * cb)
{
	return cb->count == cb->size;
}


int cbuffer_is_empty(const struct cbuffer * cb)
{
	return cb->count == 0;
}


void cbuffer_write(struct cbuffer * cb, void * elem)
{
	int end = (cb->start + cb->count) % cb->size;
	memmove(&cb->data[end * cb->elemsize], elem, cb->elemsize);
	if (cb->count == cb->size) {
		cb->start = (cb->start + 1) % cb->size; /* full, overwrite */
	} else {
		++cb->count;
	}
}


void cbuffer_read(struct cbuffer * cb, void * elem)
{
	memmove(elem, &cb->data[cb->start * cb->elemsize], cb->elemsize);
	cb->start = (cb->start + 1) % cb->size;
	--cb->count;
}
