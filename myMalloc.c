#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<string.h>

#define ALIGNMENT 8

uint8_t MY_HEAP[64000];

typedef struct {

    int blockSize;
    uint8_t *nextFree;
    uint8_t *prevFree;

} Metadata;

/* DEBUG */
static void printMem(int end) {

    printf("\n");
    for (int i = 0; i < end; i++) printf("%d ", MY_HEAP[i]);
    printf("\n");
}

static void place_block(uint8_t *ptr, size_t size) {

    int newsize = size + 1;               
    int oldsize = *ptr & ~0x1;           

    if (newsize < oldsize) {           

        *ptr = newsize | 0x1;               
        *(ptr + newsize) = oldsize - newsize;
    }
}

static size_t ClosestMultipleOfEight(size_t n) {

    if (n % 8 != 0) {

        return ((n / 8)+1) * 8;  // Add padding
    } 

    return n;
}

/* Setup initial memory */
static void init() {

    // Set all values to zero
    memset(&MY_HEAP, 0x00, 64000 * sizeof(uint8_t));

    // Add first header
    Metadata *firstBlock = (Metadata *) MY_HEAP;
    firstBlock->blockSize = ClosestMultipleOfEight(sizeof(Metadata));

}

void *my_malloc(size_t size) {

    // Set new size of block for alignment
    size_t newSize = ClosestMultipleOfEight(size);

    printf("New size for malloc(%d) is %d\n", size, newSize);

    // Find best fit
    size_t minSize = INT32_MAX;
    uint8_t *address = NULL; // The address at which to put the block
    Metadata *cursor = (Metadata *) MY_HEAP;
    while (cursor->nextFree != NULL) {

        printf("Current : %p\n", cursor);
        // If size is exact, stop the loop
        if (newSize == cursor->blockSize) {

            minSize = cursor->blockSize;
            address = (uint8_t *) cursor;
            break;
        } 

        // Find smallest fitting block
        if (newSize < cursor->blockSize) {

            minSize = cursor->blockSize;
            address = (uint8_t *) cursor;
        } 

        cursor = (Metadata *) cursor->nextFree;
    }
    printf("The address is %p with min size %d\n", address, minSize);

    // Add header
    // Add footer
    // Return address

    return NULL;
}

void my_free(void *ptr) {

    // Check if pointer exists with header

    return;
}

int main(int argc, char *argv[]) {

    init(); // Call to init() function to set memory to initial state

    printf("Address of heap : %p\n", MY_HEAP);
    uint8_t *ptr = my_malloc(2);
    printMem(50);
    return EXIT_SUCCESS;
}
