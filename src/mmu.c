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

#define ISNULL(a) ((!a) && (a->ptr == null))
#define ISNULLDOT(a) ((!a.ptr) && (a.ptr == null))

void bp_mmu_init(const unsigned long long _size) {
    table_size = _size / 8;
    if ((_size & 7) != 0) table_size = (_size / 8) + 1;

#ifndef BP_NO_WINDOW
    if (_size <= 520 + table_size) {
        print("[BPE MMU WARNING] Not enough memory for the engine to run. Required memory (in bytes): \n"
              "Input Manager: 520 \n"
              "Screen buffer: {window width * window height * 4}\n"
              "Memory Management Unit Metadata: {Allocated memory / 8}\n"
              "IMPORTANT NOTICE: \n"
              "These values do not account for the compiler's memory usage. Actual memory usage might exceed the specified value\n");
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
    if (ISNULL(ptr)) {
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
    if (ISNULL(ptr)) {
        print("[BP MMU ERROR] Error on bp_memset(), null pointer or size 0");
        return;
    }

    u64 bytes_to_write = _size;
    if (bytes_to_write == 0|| bytes_to_write > ptr->size) bytes_to_write = ptr->size;

    byte* dst = (byte*)ptr->ptr;

    for (u64 i = 0; i < bytes_to_write; i++) dst[i] = value;
}

void bp_memcpy(const bp_ptr* dst, const bp_ptr* src, const u64 _size) {
    if (ISNULL(dst) || ISNULL(src)) {
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
    if (ISNULL(dst) || ISNULL(src)) {
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

i32 bp_memcmp(const bp_ptr* ptr1, const bp_ptr* ptr2, const u64 _size) {
    if (ISNULL(ptr1) || ISNULL(ptr2)) {
        print("[BP MMU ERROR] Error on bp_memcmp() null pointer");
        return ~0;
    }

    u64 cmp_bytes = _size;
    if (cmp_bytes > ptr1->size) cmp_bytes = ptr1->size;
    if (cmp_bytes > ptr2->size) cmp_bytes = ptr2->size;

    const byte* p1 = (const byte*)ptr1->ptr;
    const byte* p2 = (const byte*)ptr1->ptr;

    for (u64 i = 0; i < cmp_bytes; i++) if (p1[i] != p2[i]) return (i32)p1[i] - (i32)p2[i];

    return 0;
}

bp_ptr bp_realloc(bp_ptr* ptr, const u64 new_size) {
    if (ISNULL(ptr)) return bp_malloc(new_size);

    if (new_size == 0) {
        bp_free(ptr);
        return bp_nullptr;
    }

    if (new_size == ptr->size) return *ptr;

    const byte* data_start = (byte*)mem_start + ptr->size;
    const u64 byte_offset = (u8*)ptr->ptr - data_start;
    const u64 start_bit_index = byte_offset / BLOCK_SIZE;

    if (new_size < ptr->size) {
        const u64 bits_to_free = ptr->size - new_size;
        const u64 free_start_bit = start_bit_index + new_size;

        for (u64 i = free_start_bit; i < free_start_bit + bits_to_free; i++) clear_bit(i);

        ptr->size = new_size;
        return *ptr;
    }

    const u64 needed_bits = new_size - ptr->size;
    const u64 check_start_bit = start_bit_index + ptr->size;

    bool can_expand = true;

    if (check_start_bit + needed_bits <= table_size * 8) {
        for (u64 i = check_start_bit; i < check_start_bit + needed_bits; i++)
            if (is_bit_set(i)) {
                can_expand = false;
                break;
            }
    }
    else can_expand = false;

    if (can_expand) {
        for (u64 i = check_start_bit; i < check_start_bit + needed_bits; i++) set_bit(i);
        ptr->size = new_size;
        return *ptr;
    }

    // Fallback
    bp_ptr new_ptr = bp_malloc(new_size);
    if (ISNULLDOT(new_ptr)) {
        print("[BP MMU ERROR] Error on bp_realloc. Not enough memory to resize");
        return *ptr;
    }


    bp_memcpy(&new_ptr, ptr, ptr->size);
    bp_free(ptr);

    return new_ptr;
}

bool bp_read_file(const char *file_path, bp_ptr* out_buffer, u64* out_size) {
#ifdef _WIN32
    HANDLE hFile = CreateFileA(
        file_path,
        GENERIC_READ,
        FILE_SHARE_READ, NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) return false;

    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(hFile, &file_size)) {
        CloseHandle(hFile);
        return false;
    }

    bp_ptr buffer_ptr = bp_malloc(file_size.QuadPart + 1);
    if (!buffer_ptr.ptr) {
        CloseHandle(hFile);
        return false;
    }

    u64 total_read = 0;
    while (total_read < (u64)file_size.QuadPart) {
        u64 remaining = (u64)file_size.QuadPart - (u64)total_read;

        DWORD to_read = (remaining > 0xFFFFFFFF) ? 0xFFFFFFFF : (DWORD)remaining;
        DWORD bytes_read = 0;

        if (!ReadFile(hFile, buffer_ptr.ptr + total_read, to_read, &bytes_read, NULL) || bytes_read == 0) break;
        total_read += bytes_read;
    }

    CloseHandle(hFile);

    *((char*)buffer_ptr.ptr + total_read) = '\0';

    out_buffer->ptr = buffer_ptr.ptr;
    if (out_size) *out_size = total_read;
    return true;
#else
    return false;
#endif
}