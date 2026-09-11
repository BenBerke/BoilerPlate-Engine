//
// Created by berke on 9/11/2026.
//

#ifndef BPENGINE_MMU_H
#define BPENGINE_MMU_H

#include "typedefs.h"

typedef struct bp_ptr {
    void* ptr;
    u64 size;
} bp_ptr;

void bp_mmu_init(unsigned long long _size);

byte* bp_get_mem_start();
u64 bp_get_mem_table_size();

bp_ptr bp_malloc(u64 _size);
void bp_free(bp_ptr* ptr);
void bp_memset(const bp_ptr* ptr, i8 value, u64 _size);
void bp_memcpy(const bp_ptr* dst, const bp_ptr* src, u64 _size);

#endif //BPENGINE_MMU_H