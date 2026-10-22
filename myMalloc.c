#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<string.h>

#define ALIGNMENT 8

uint8_t MY_HEAP[64000];

typedef struct metadata {

    int blockSize;
    uint8_t *nextFree;
    uint8_t *prevFree;

} Metadata;

/* DEBUG */
static void printMem(int end) {

    for (int i = 0; i < end; i++) printf("%d ", MY_HEAP[i]);
}

static void place_block(uint8_t *ptr, size_t size) {

    int newsize = size + 1;               
    int oldsize = *ptr & ~0x1;           

    if (newsize < oldsize) {           

        *ptr = newsize | 0x1;               
        *(ptr + newsize) = oldsize - newsize;
    }
}

/* Setup initial memory */
static void init() {

    // Set all values to zero
    memset(&MY_HEAP, 0x00, 64000 * sizeof(uint8_t));

    // Add first header
    Metadata *firstBlock = (Metadata *) MY_HEAP;
    firstBlock->blockSize = (sizeof(Metadata) / 8) * 8;
    firstBlock->nextFree = (uint8_t *) firstBlock + firstBlock->blockSize + 1; 
    firstBlock->prevFree = NULL;

    return;
}

void *my_malloc(size_t size) {

    // Set new size of block for alignment
    size_t newSize = size;
    if (size % 8 != 0) {

        newSize = (((int) size / 8)+1) * 8;  // Add padding
    } 

    printf("New size for malloc(%d) is %d\n", size, newSize);

    return NULL;
}

void my_free(void *ptr) {

    // Check if pointer exists with header

    return;
}

int main(int argc, char *argv[]) {

    init(); // Call to init() function to set memory to initial state
    uint8_t *ptr = my_malloc(2);
    uint8_t *ptr1 = my_malloc(9);
    uint8_t *ptr2 = my_malloc(13);
    uint8_t *ptr3 = my_malloc(16);
    //printf("Adress of ptr : %p, value of size : %d\n", ptr, *(ptr-3));
    printMem(50);
    return EXIT_SUCCESS;
}
