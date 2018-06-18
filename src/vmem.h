#ifndef VMEM_H_
#define VMEM_H_

struct mem_range {
	long start, size;
	struct mem_range *prev, *next;
};

enum {
	TEX_MEM,
	PAL_MEM,

	NUM_VMEM_POOLS
};

struct mem_range *ds3_alloc_vmem(int pool, int sz);
void ds3_free_vmem(int pool, struct mem_range *mem);

#endif	/* VMEM_H_ */
