#include <iostream>
#include <unistd.h>   // sbrk
#include <cstddef>
#include <cstdint>
#include <cassert>
using namespace std;

enum valid_state {
    Used,
    Free
};

struct Metadata {
    size_t size;
    valid_state State;
};

void* heap_start = nullptr;
void* heap_end   = nullptr;

// Align helper (using % as you prefer)
size_t align_up(size_t x, size_t align) {
    size_t rem = x % align;
    if (rem == 0) return x;
    return x + (align - rem);
}

void* Mylloc(size_t req_size) {
    if (!heap_start) {
        heap_start = sbrk(0);   // get initial heap end
        heap_end   = heap_start;
    }

    Metadata* traveller = (Metadata*)heap_start;

    while ((void*)traveller < heap_end) {
        if (traveller->State == Free && traveller->size >= req_size) {

            // split if enough space remains
            if (traveller->size >= req_size + sizeof(Metadata) + 1) {
                Metadata* next =
                    (Metadata*)((char*)traveller + sizeof(Metadata) + req_size);

                next->State = Free;
                next->size =
                    traveller->size - req_size - sizeof(Metadata);
            }

            traveller->size = req_size;
            traveller->State = Used;
            return (char*)traveller + sizeof(Metadata);
        }

        traveller = (Metadata*)((char*)traveller +
                     sizeof(Metadata) + traveller->size);
    }

    size_t alignment = alignof(std::max_align_t);
    size_t total_space = sizeof(Metadata) + req_size;

    // Align heap_end
    uintptr_t curr = (uintptr_t)heap_end;
    size_t aligned = align_up(curr, alignment);
    size_t padding = aligned - curr;

    // Request memory from OS
    void* request = sbrk(padding + total_space);
    if (request == (void*)-1) {
        cout << "sbrk failed: out of memory\n";
        return nullptr;
    }

    Metadata* header = (Metadata*)((char*)heap_end + padding);
    header->size = req_size;
    header->State = Used;

    heap_end = (char*)heap_end + padding + total_space;

    return (char*)header + sizeof(Metadata);
}

void Myfree(void* user_ptr) {
    if (!user_ptr) return;

    Metadata* header =
        (Metadata*)((char*)user_ptr - sizeof(Metadata));
    header->State = Free;

    // COALESCE RIGHT 
    Metadata* right =
        (Metadata*)((char*)header + sizeof(Metadata) + header->size);

    if ((void*)right < heap_end && right->State == Free) {
        header->size += sizeof(Metadata) + right->size;
    }

    // COALESCE LEFT 
    Metadata* left = (Metadata*)heap_start;
    Metadata* prev = nullptr;

    while ((void*)left < (void*)header) {
        prev = left;
        left = (Metadata*)((char*)left +
                 sizeof(Metadata) + left->size);
    }

    if (prev && prev->State == Free) {
        prev->size += sizeof(Metadata) + header->size;
    }
}

