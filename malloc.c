#include <stdio.h> //printf()
#include <stddef.h> //size_t
#include <sys/mman.h> //mmap()
#include "malloc.h"//heapInit(), malloc(), free(), printFreeList()

typedef struct node{		//defines a node structure to be used in the free list
	int size;				//the size of memory that can be allocated from a node, excluding the size of the node itself
	struct node *next
}node_t;
typedef struct header{		//defines a header that is used to manage the amount of memory allocated
	int size;
	int magic;
}header_t;

node_t *head = NULL;
node_t *current = NULL;

/*
 * heapInit - initializes a heap of totalHeapSize
 * embeds the free list within the heap by setting the head of the list as the first memory address
 * in the heap. Adjusts the size of the heap to be totalHeapSize minus the size of a node_t.
 */
int heapInit(size_t totalHeapSize){	
	head = mmap(NULL, totalHeapSize, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
	head->size = totalHeapSize - sizeof(node_t);
	head->next = NULL;
	return 0;
}
/*
 * malloc - returns a void pointer that points to a block of memory of the 
 * requested size. allocates a header before the block of memory that will
 * contain the size of the allocated block, that can later be used when the block 
 * is freed.
 */
void* malloc(size_t size){
	void* ptr = NULL;
	if(size <= 0){
		return ptr = NULL;
	}
	current = head;
	while(current->size < (size + sizeof(header_t)) && (current != NULL)){
		current = current->next;
		if(current == NULL){
			return ptr = NULL;
		}
	}
	if (current->size == (size + sizeof(header_t))){
		ptr = (void*)((void*)current + sizeof(header_t));
		header_t *ptrHdr = (void*)((void*) ptr - sizeof(header_t));
		ptrHdr->size = size;
		current->size = current->size - size - sizeof(header_t);
	}else if(current->size > (size + sizeof(header_t))){
		ptr = (void*)((void*)current + current->size - size + sizeof(header_t));
		header_t *ptrHdr = (void*)((void*)ptr - sizeof(header_t));
		ptrHdr->size = size;
		current->size = current->size - size - sizeof(header_t);
	}else{
		ptr = NULL;
	} return ptr;
}
/*
 * free - takes a void pointer as an argument and frees the memory
 * associated with that pointer. finds the header for the pointer and
 * creates a new node with a size that is equal to the size of the freed	
 * block of memory.
 */
void free(void* ptr){
	if(ptr == NULL){
		return;
	}
	
	header_t *ptrHdr = (void*)((void*)ptr - sizeof(header_t));
	node_t *newNode = (void*)ptrHdr;
	newNode->size = ptrHdr->size;
	newNode->next = head;
	head = newNode;
}
/*
 * printFreeList - prints the free blocks of memory cntained within the free list
 */
void printFreeList(){
	current = head;
	printf("Free List:\n");
	while(current != NULL){
		printf("Free chunk at %p (size: %i, next: %p)\n", current, current->size, current->next);
		current = current->next;
	} printf("\n");
}

