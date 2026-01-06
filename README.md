# Custom Heap Memory Allocator (C++ / Linux)

## Overview
This project implements a custom dynamic memory allocator in C++ using the Linux `sbrk` system call. The allocator manually manages heap memory using metadata headers and reuses freed blocks to reduce fragmentation, demonstrating low-level memory management on Linux.

## Features Implemented
- Dynamic heap growth using `sbrk`
- Metadata-based heap block management
- Implicit free-list traversal
- Reuse of freed memory blocks
- Block splitting during allocation
- Left and right block coalescing during deallocation
- Manual pointer arithmetic and heap traversal

## Design Notes
- Uses an implicit free-list approach by linearly traversing heap blocks

## Planned Enhancements
- Heap corruption detection using magic numbers (canaries)
- Double-free and invalid pointer detection
- `mmap` support for large allocations
- Optional performance optimizations such as segregated free lists
