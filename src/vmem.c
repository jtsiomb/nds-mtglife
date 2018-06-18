#include <stdlib.h>
#include <limits.h>
#include "vmem.h"
#include "ds.h"

static struct mem_range *free_list[NUM_VMEM_POOLS];

struct mem_range *ds3_alloc_vmem(int pool, int sz)
{
	struct mem_range *res, *m = free_list[pool];

	while(m) {
		if(m->size == sz) {
			/* found a block exactly the right size, rip the node off and return it */
			if(m->prev) {
				m->prev->next = m->next;
			} else {
				free_list[pool] = m->next;
			}
			if(m->next) m->next->prev = m->prev;
			m->prev = m->next = 0;
			return m;
		}

		if(m->size > sz) {
			/* found a block big enough, carve a piece and return new node */
			if(!(res = malloc(sizeof *res))) {
				ds_panic("alloc_tex_mem: failed to allocate mem_range node");
				return 0;
			}
			res->start = m->start;
			res->size = sz;
			m->start += sz;
			m->size -= sz;
			return res;
		}

		m = m->next;
	}

	return 0;
}

void ds3_free_vmem(int pool, struct mem_range *mem)
{
	struct mem_range *m, *mnext;
	struct mem_range dummy;

	if(!free_list[pool]) {
		free_list[pool] = mem;
		return;
	}

	dummy.start = LONG_MIN;
	dummy.size = 0;
	dummy.prev = 0;
	dummy.next = free_list[pool];

	/* find where to put it */
	m = free_list[pool];
	while(m->next && m->next->start < mem->start) {
		m = m->next;
	}
	mnext = m->next;

	if(m->start + m->size == mem->start) {
		if(mnext && mem->start + mem->size == mnext->start) {
			/* coalesce both sides */
			m->size += mem->size + mnext->size;
			m->next = mnext->next;
			if(mnext->next) mnext->next->prev = m;
			free(mem);
			free(mnext);
			return;
		}

		/* coalesce low */
		m->size += mem->size;
		free(mem);
		return;
	}

	if(mem->start + mem->size == mnext->start) {
		/* coalesce high */
		mnext->size += mem->size;
		mnext->start = mem->start;
		free(mem);
		return;
	}

	/* disjoint, just link it in */
	mem->prev = m;
	mem->next = mnext;
	m->next = mem;
	if(mnext) mnext->prev = mem;

	/* if we inserted it at the very begining, update global pointer */
	if(mem->prev == &dummy) {
		mem->prev = 0;
		free_list[pool] = dummy.next;
	}
}
