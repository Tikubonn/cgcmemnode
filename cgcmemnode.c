
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "memnode.h"
#include "bitarray.h"
#include "cgcmemnode.h"

#ifndef MIN
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a, b) ((a)<(b)?(b):(a))
#endif

bool cgcmemnode_history_recordp (void *address, cgcmemnode_history *history){
  for (cgcmemnode_history *hist = history; hist != NULL; hist = hist->previous){
    if (hist->address == address){ return true; }
  }
  return false;
}

size_t cgcmemnode_require_counter_length (size_t size, size_t resolution){
  return CGCMEMNODE_REQUIRE_COUNTER_LENGTH(size, resolution);
}

static void cgcmemnode_clear_counterseq (cgcmemnode_counter *sequence, size_t length){
  for (size_t index = 0; index < length; index++){
    sequence[index] = 0;
  }
}

void cgcmemnode_init (void *sequence, size_t size, cgcmemnode_counter *counterseq, size_t counterlen, size_t counterresolution, bitarray *reservemask, cgcmemnode *nextnode, cgcmemnode *cmemnode){
  cgcmemnode_clear_counterseq(counterseq, counterlen);
  memnode_init(sequence, size, reservemask, (memnode*)nextnode, &(cmemnode->memnode));
  cmemnode->counterseq = counterseq;
  cmemnode->counterlen = counterlen;
  cmemnode->counterresolution = counterresolution;
}

cgcmemnode *make_cgcmemnode (size_t size, size_t resolution, cgcmemnode *nextnode){
  size_t requirecounterlength = cgcmemnode_require_counter_length(size, resolution);
  cgcmemnode *cmemnode = malloc(sizeof(cgcmemnode));
  if (cmemnode == NULL){
    return NULL;
  }
  void *cmemnodeseq = malloc(size);
  if (cmemnodeseq == NULL){
    free(cmemnode);
    return NULL;
  }
  cgcmemnode_counter *cmemnodecounterseq = malloc(sizeof(cgcmemnode_counter) * requirecounterlength);
  if (cmemnodecounterseq == NULL){
    free(cmemnode);
    free(cmemnodeseq);
    return NULL;
  }
  bitarray *cmemnodereservemask = make_bitarray(size);
  if (cmemnodereservemask == NULL){
    free(cmemnode);
    free(cmemnodeseq);
    free(cmemnodecounterseq);
    return NULL;
  }
  cgcmemnode_init(cmemnodeseq, size, cmemnodecounterseq, requirecounterlength, resolution, cmemnodereservemask, nextnode, cmemnode);
  return cmemnode;
}

void free_cgcmemnode (cgcmemnode *cmemnode){
  cgcmemnode *cmnode = cmemnode;
  while (cmnode != NULL){
    cgcmemnode *nextnode = cgcmemnode_next(cmnode);
    free_bitarray(cmnode->memnode.reservemask);
    free(cmnode->memnode.sequence);
    free(cmnode->counterseq);
    free(cmnode);
    cmnode = nextnode;
  }
}

memnode *cgcmemnode_memnode (cgcmemnode *cmemnode){
  return &(cmemnode->memnode);
}

cgcmemnode *cgcmemnode_next (cgcmemnode *cmemnode){
  return (cgcmemnode*)memnode_next(cgcmemnode_memnode(cmemnode));
}

static int cgcmemnode_find (void *address, size_t size, cgcmemnode *cmemnode, cgcmemnode **foundcmemnodep, size_t *cstartp, size_t *cendp){
  memnode *foundmemnode = memnode_find(address, cgcmemnode_memnode(cmemnode));
  cgcmemnode *foundcmemnode = (cgcmemnode*)foundmemnode;
  if (foundmemnode == NULL){ return 1; }
  size_t start = address - foundmemnode->sequence;
  size_t end = start + size;
  *foundcmemnodep = foundcmemnode;
  *cstartp = start / foundcmemnode->counterresolution;
  *cendp = end / foundcmemnode->counterresolution + (0 < end % foundcmemnode->counterresolution? 1: 0);
  return 0;
}

int cgcmemnode_increase (void *address, size_t size, cgcmemnode *cmemnode){
  cgcmemnode *foundcmemnode;
  size_t cstart;
  size_t cend;
  if (cgcmemnode_find(address, size, cmemnode, &foundcmemnode, &cstart, &cend) != 0){ return 1; }
  for (size_t index = cstart; index < cend; index++){
    if (foundcmemnode->counterseq[index] < CGCMEMNODE_COUNTER_MAX){
      foundcmemnode->counterseq[index] += 1;
    }
    else {
      return 1;
    }
  }
  return 0;
}

int cgcmemnode_decrease (void *address, size_t size, cgcmemnode *cmemnode){
  cgcmemnode *foundcmemnode;
  size_t cstart;
  size_t cend;
  if (cgcmemnode_find(address, size, cmemnode, &foundcmemnode, &cstart, &cend) != 0){ return 1; }
  for (size_t index = cstart; index < cend; index++){
    if (0 < foundcmemnode->counterseq[index]){
      foundcmemnode->counterseq[index] -= 1;
      if (foundcmemnode->counterseq[index] == 0){
        if (bitarray_fill(0, index * foundcmemnode->counterresolution, MIN(index * foundcmemnode->counterresolution + foundcmemnode->counterresolution, bitarray_length(foundcmemnode->memnode.reservemask)), foundcmemnode->memnode.reservemask) != 0){ return 1; }
      }
    }
    else {
      return 1;
    }
  }
  return 0;
}

void *cgcmemnode_allocate (size_t size, cgcmemnode *cmemnode){
  void *address = memnode_allocate(size, cgcmemnode_memnode(cmemnode));
  if (address != NULL){
    if (cgcmemnode_increase(address, size, cmemnode) != 0){ return NULL; }
  }
  return address;
}
