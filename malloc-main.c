#include <stdlib.h> // strtol()
#include <stdio.h>  // fprintf(), etc
#include <stddef.h>  // size_t
#include <errno.h>  // errno
#include "malloc.h" // heapInit(), malloc(), free()

// Parse command-line arguments
void parseCommandlineArgs(int argc, char* argv[], size_t* size, int* seed){

    if(argc >= 2){
        // get the initial heap size from the first command-line argument
        char *end; // for determining if a number was read in correctly
        *size = (size_t) strtol(argv[1], &end, 10);

        if (*end != '\0' || errno != 0){
            fprintf(stderr, "\nERROR: Unable to parse total heap size value from command-line arguments!\n\n");
            fprintf(stderr, "Usage: argv[0] [<total heap size>] [<seed>]>\n");
            exit(1);
        }
    
        if(argc >= 3){
            *seed = (int) strtol(argv[2], &end, 10);

            if (*end != '\0' || errno != 0){
                fprintf(stderr, "\nERROR: Unable to parse seed value from command-line arguments!\n\n");
                fprintf(stderr, "Usage: argv[0] [<total heap size>] [<seed>]>\n");
                exit(1);
            }
        }
    }    
}

void memoryAllocationsSimple(){
    void* chunkPointers[4];
    size_t requestedSize = 24;

    // simple allocations and deallocations
    chunkPointers[0] = malloc(requestedSize);
    if(chunkPointers[0] == NULL){
        printf( "\nALERT: malloc(%zu) returned NULL!\n\n", requestedSize);
    }

    printFreeList();

    free(chunkPointers[0]);

    printFreeList();
        
    // Verify free(NULL) doesn't crash
    free(NULL);

    printFreeList();
        
    requestedSize = 64;
    chunkPointers[1] = malloc(requestedSize);
    if(chunkPointers[1] == NULL){
        printf("\nALERT: malloc(%zu) returned NULL!\n\n", requestedSize);
    }

    printFreeList();

    requestedSize = 72;
    chunkPointers[2] = malloc(requestedSize);
    if(chunkPointers[2] == NULL){
        printf("\nALERT: malloc(%zu) returned NULL!\n\n", requestedSize);
    }

    printFreeList();

    requestedSize = 8;
    chunkPointers[3] = malloc(requestedSize);
    if(chunkPointers[3] == NULL){
        printf("\nALERT: malloc(%zu) returned NULL!\n\n", requestedSize);
    }

    printFreeList();

    free(chunkPointers[3]);
    
    printFreeList();
    
    free(chunkPointers[2]);
    
    printFreeList();
    
    free(chunkPointers[1]);
    
    printFreeList();
}


void memoryAllocationsFull(size_t heapSize, int numChunks){
    void* chunkPointers[numChunks + 1];
    size_t requestedSize = 24;

    // fill up the heap with requestedSize byte blocks, and then "+ 1" 
    for(int i = 0; i < numChunks + 1; i++){
        chunkPointers[i] = malloc(requestedSize);
        if(chunkPointers[i] == NULL){
            printf("\nALERT: malloc(%zu) returned NULL!\n\n", requestedSize);
        }

        printFreeList();
    }

    // deallocate all allocated memory
    for(int i = 0; i < numChunks; i++){
        free(chunkPointers[i]);

        printFreeList();
    }
}


void memoryAllocationsRandom(unsigned int seed){
    int numPtrs = 10;
    void* chunkPointers[numPtrs];
    int numCallPairs = numPtrs * 10;
    
    // initialize allocated memory pointers
    for( int i = 0; i < numPtrs; i++){
        chunkPointers[i] = NULL;
    }
    
    // Randomly choose between malloc() and free()
    // Randomly choose an index where to store it (allows for memory leaks)
    srandom(seed);  // seed the random number generator

    for(int i = 0; i < numCallPairs; i++){
        int randIndex = random() % numPtrs;
        long randFunc = random() & 0x3; // values 0, 1, 2, and 3
        if(randFunc == 0){  // 1 in 4 chance
            int randSize = ((random() % 100) + 1) * 8; // values between 8 and 800 (but only multiples of 8)
            chunkPointers[ randIndex ] = malloc(randSize);
            if(chunkPointers[randIndex] == NULL){
                printf("\nALERT: malloc( %d ) returned NULL!\n\n", randSize);
            }
        }else{
            free(chunkPointers[randIndex]);
            chunkPointers[randIndex] = NULL;  // prevents a double-free
        }
        if((random() & 0x7) == 0){
            // 1 in 8 chance
            printFreeList();
        }
    }

    // deallocate all allocated memory
    for(int i = 0; i < numPtrs; i++){
        if(chunkPointers[i] != NULL){
            free(chunkPointers[i]);
        }
    }
    
    printFreeList();
}


int main(int argc, char* argv[]){
    size_t initHeapSize = 128; // initial (total) heap size (bytes)
    int seed = -1;      // seed for random number generator (or 0 if not used)

    parseCommandlineArgs(argc, argv, &initHeapSize, &seed);

    // initialize the heap
    if(heapInit(initHeapSize) != 0){
        fprintf(stderr, "\nERROR: heapInit(%zu) failed!\n\n", initHeapSize);
        return 1;
    }
	
    printFreeList();

    /* Allocate and de-alloc memory */
    if(seed == -1){
        memoryAllocationsSimple();
    }else if(seed == -2){
        int numChunks = (initHeapSize - 16) / (8 + 24); // 1 free block with the maximum number of 24-byte allocated chunks
        memoryAllocationsFull(initHeapSize, numChunks);
    }else{
        memoryAllocationsRandom(seed);
    }
    
    return 0;
}
