//
// Created by berke on 9/11/2026.
//

#include "../headers/mmu.h"
#include "../headers/app.h"
#include "../headers/debug.h"
#include "../headers/config.h"

static byte* mem_start;
static unsigned long long size;
static u64 table_size; // table start = mem_start
#define BLOCK_SIZE 1 // bytes of memory per table bit

void bp_mmu_init(const unsigned long long _size) {
    table_size = _size / 8;
    if ((_size & 7) != 0) table_size = (_size / 8) + 1;

#ifndef BP_NO_WINDOW
    if (_size <= 520 + table_size) {
        print("[BPE MMU WARNING] Not enough memory for the engine to run. Required memory (in bytes): \n"
              "Input Manager: 520 \n"
              "Screen buffer: {window width * window height * 4}\n"
              "Memory Management Unit Metadata: {Allocated memory / 8}\n");
        return;
    }
#else
    if (_size == 0) {
        print("[BPE MMU WARNING] Application can not start with 0 or less memory");
    }
#endif

#ifdef _WIN32
    mem_start = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, _size);
#else
    mem_start = (byte*)calloc(1, _size);
#endif

    size = _size;
}

byte* bp_get_mem_start() { return mem_start; }
u64 bp_get_mem_table_size() { return table_size; }

static inline bool is_bit_set(const u64 bit_index) {
    return (mem_start[bit_index / 8] & (1 << (bit_index % 8))) != 0;
}

static inline void set_bit(const u64 bit_index) {
    mem_start[bit_index / 8] |= (1 << (bit_index % 8));
}

static inline void clear_bit(const u64 bit_index) {
    mem_start[bit_index / 8] &= ~(1 << (bit_index % 8));
}

bp_ptr bp_malloc(const u64 _size) {
    bp_ptr result = { null, 0};
    const u64 req_bits = (_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (req_bits == 0) { print("[BP MMU ERROR] Can not malloc 0 bytes"); return result; }

    u64 cont_free = 0;
    u64 start_bit_index = 0;
    for (int i = 0; i < (table_size * 8); i++) {
        // Fast skip fully occupied bytes
        if (cont_free == 0 && (i % 8 == 0) && ((i / 8) < table_size)) if (mem_start[i / 8] == 0xFF) { i += 7; continue; }

        if (!is_bit_set(i)) {
            if (cont_free == 0) start_bit_index = i;
            cont_free++;

            if (cont_free == req_bits) {
                for (u64 j = start_bit_index; j < start_bit_index + req_bits; j++) set_bit(j);
                result.ptr = (void*)((mem_start + table_size) + (start_bit_index * BLOCK_SIZE));
                result.size = _size;
                return result;
            }
        } else cont_free = 0;
    }

    print("[BP MMU ERROR] Not enough memory or memory too fragmented");
    return result;
}

void bp_free(bp_ptr* ptr) {
    if (ptr->ptr == null || ptr->size == 0) {
        print("[BP MMU ERROR] Error on bp_free(), null pointer or size 0");
        return;
    }

    const u64 req_bits = ptr->size * 8;
    const byte* data_start = (byte*)mem_start + table_size;
    const u64 byte_offset = (u8*)ptr->ptr - data_start;
    const u64 start_bit_index = byte_offset / BLOCK_SIZE;

    for (u64 i = start_bit_index; i < start_bit_index + req_bits; i++) clear_bit(i);
    ptr->ptr = null;
    ptr->size = ~0;
}

void bp_memset(const bp_ptr* ptr, const i8 value, const u64 _size) {
    if (ptr->ptr == null || ptr->size == 0) {
        print("[BP MMU ERROR] Error on bp_memset(), null pointer or size 0");
        return;
    }

    u64 bytes_to_write = _size;
    if (bytes_to_write == 0|| bytes_to_write > ptr->size) bytes_to_write = ptr->size;

    byte* dst = (byte*)ptr->ptr;

    for (u64 i = 0; i < bytes_to_write; i++) dst[i] = value;
}

void bp_memcpy(const bp_ptr* dst, const bp_ptr* src, const u64 _size) {
    if (!dst || !src || dst->ptr == 0 || src->ptr == null) {
        print("[BP MMU ERROR] Error on bp_memcpy() null pointer");
        return;
    }

    u64 copy_bytes = _size;
    if (copy_bytes > dst->size) copy_bytes = dst->size;
    if (copy_bytes > src->size) copy_bytes = src->size;

    byte* d = (byte*)dst->ptr;
    const byte* s = (const byte*)src->ptr;

    for (u64 i = 0; i < copy_bytes; i++) d[i] = s[i];
}

void bp_memmove(bp_ptr* dst, bp_ptr* src, const u64 _size) {
    if (!dst || !src || dst->ptr == 0 || src->ptr == null) {
        print("[BP MMU ERROR] Error on bp_memmove() null pointer");
        return;
    }

    u64 copy_bytes = _size;
    if (copy_bytes > dst->size) copy_bytes = dst->size;
    if (copy_bytes > src->size) copy_bytes = src->size;

    byte* d = (byte*)dst->ptr;
    const byte* s = (const byte*)src->ptr;

    if (d < s)  for (u64 i = 0; i < copy_bytes; i++) d[i] = s[i];
    else for (u64 i = 0; i < copy_bytes; i--) d[i - 1] = s[i - 1];
}