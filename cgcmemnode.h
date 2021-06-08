
#pragma once
#include <stdint.h>
#include <stddef.h>
#include "memnode.h"
#include "bitarray.h"

typedef unsigned short cgcmemnode_counter;

typedef struct cgcmemnode {
  memnode memnode;
  cgcmemnode_counter *counterseq;
  size_t counterlen;
  size_t counterresolution;
} cgcmemnode;

typedef struct cgcmemnode_history {
  void *address;
  struct cgcmemnode_history *previous;
} cgcmemnode_history;

#define CGCMEMNODE_DEFAULT_RESOLUTION (sizeof(max_align_t))
#define CGCMEMNODE_TO_STRING(n) n
#define CGCMEMNODE_GENSYM CGCMEMNODE_ ## CGCMEMNODE_TO_STRING(__LINE__)

#define CGCMEMNODE_REQUIRE_COUNTER_LENGTH(size, resolution)\
(((size) / (resolution)) + (0 < (size) % (resolution)? 1: 0))

#define MAKE_LOCAL_CGCMEMNODE(varname, size, resolution, nextnode)\
MAKE_LOCAL_BITARRAY(__ ## varname ## _reservemask, size);\
uint8_t __ ## varname ## _sequence[(size)] = {};\
cgcmemnode_counter __ ## varname ## _counterseq[CGCMEMNODE_REQUIRE_COUNTER_LENGTH(size, resolution)] = {};\
cgcmemnode __ ## varname = { { __ ## varname ## _reservemask, __ ## varname ## _sequence, (size), (nextnode) }, __ ## varname ## _counterseq, CGCMEMNODE_REQUIRE_COUNTER_LENGTH(size, resolution), (resolution) };\
cgcmemnode *varname = &__ ## varname;

#define MAKE_CGCMEMNODE_HISTORY(varname)\
cgcmemnode_history *varname = NULL;

#define ADD_CGCMEMNODE_HISTORY(address, varname)\
cgcmemnode_history CGCMEMNODE_GENSYM ## _ ## varname = { address, varname };\
varname = &CGCMEMNODE_GENSYM ## _ ## varname;

extern bool cgcmemnode_history_recordp (void*, cgcmemnode_history*);
extern size_t cgcmemnode_require_counter_length (size_t, size_t);
extern void cgcmemnode_init (void*, size_t, cgcmemnode_counter*, size_t, size_t, bitarray*, cgcmemnode*, cgcmemnode*);
extern cgcmemnode *make_cgcmemnode (size_t, size_t, cgcmemnode*);
extern void free_cgcmemnode (cgcmemnode*);
extern memnode *cgcmemnode_memnode (cgcmemnode*);
extern cgcmemnode *cgcmemnode_next (cgcmemnode*);
extern int cgcmemnode_increase (void*, size_t, cgcmemnode*);
extern int cgcmemnode_decrease (void*, size_t, cgcmemnode*);
extern void *cgcmemnode_allocate (size_t, cgcmemnode*);
