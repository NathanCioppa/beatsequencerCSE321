#ifndef MEM_H
#define MEM_H

#define MAX_MEM_SEQ 511
#define INCREMENT_TARGET_MEM_SEQ(x) target_mem_seq = (target_mem_seq+x) & MAX_MEM_SEQ;

#include <inttypes.h>

void store_sequence(uint16_t mem_seq);
void load_sequence(uint16_t mem_seq);
void increment_target_mem_seq(uint16_t inc_amount);

extern uint16_t target_mem_seq;

#endif