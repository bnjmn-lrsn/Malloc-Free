#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>  // size_t

// Initialize the heap to the totalHeapSize bytes (includes any header overhead)
int heapInit( size_t totalHeapSize );

// Finds an available chunk of memory that can support a payload of at least
// size bytes and reserves that memory.  Otherwise, returns NULL.
void* malloc( size_t size );

// Releases the memory block that begins at ptr.
void free( void* ptr );

// A helper function to display the details (memory address, size and next
// pointer) of each free chunk 
void printFreeList( );

#endif
