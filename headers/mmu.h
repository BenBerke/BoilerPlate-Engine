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

#define bp_nullptr ((bp_ptr){null, ~0})

void bp_mmu_init(unsigned long long _size);

byte* bp_get_mem_start();
u64 bp_get_mem_table_size();

bp_ptr bp_malloc(u64 _size);
void bp_free(bp_ptr* ptr);
void bp_memset(const bp_ptr* ptr, i8 value, u64 _size);
void bp_memcpy(const bp_ptr* dst, const bp_ptr* src, u64 _size);
i32 bp_memcmp(const bp_ptr* ptr1, const bp_ptr* ptr2, const u64 _size);
bp_ptr bp_realloc(bp_ptr* ptr, const u64 new_size);

bool bp_read_file(const char *file_path, bp_ptr* out_buffer, u64* out_size);

#endif //BPENGINE_MMU_H