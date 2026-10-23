#include <stdint.h>
#include <stdio.h>

#define HEAP_SIZE 64000
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

uint8_t MY_HEAP[HEAP_SIZE];

typedef struct {
    int blockSize;
    uint8_t *nextFree;
    uint8_t *prevFree;
} Metadata;

void init() {

    Metadata *first_block = (Metadata *)MY_HEAP;

    first_block->blockSize = HEAP_SIZE - ALIGN(sizeof(Metadata)) | 0x01; // Set last bit to 1 to mark as private
    first_block->nextFree = NULL;
    first_block->prevFree = NULL;
}

static void *place_block(uint8_t *address, int size) {
    
    // Set total size with padding
    size_t total_block_size = ALIGN(sizeof(Metadata)) + ALIGN(size);

    Metadata *cursor = (Metadata *) MY_HEAP;
    if (cursor->blockSize > total_block_size) { // If we have too much space, split memory in two

        // Create metadata for next free block
        Metadata *new_free_meta = (Metadata *) cursor + total_block_size;

        new_free_meta->blockSize = cursor->blockSize - total_block_size;
        new_free_meta->nextFree = cursor->nextFree;
        new_free_meta->prevFree = cursor->prevFree;

        printf("Successfully created new block at address %p of size %d, with nextFree %p and prevFree %p\n", new_free_meta, new_free_meta->blockSize, new_free_meta->nextFree, new_free_meta->prevFree); 

        // Set next block's previous to next free block (inserting in linked list basically)
        if (cursor->nextFree != NULL) {
            ((Metadata *)(new_free_meta->nextFree))->prevFree = (uint8_t *) new_free_meta;
        }

        // Set prev block's next to next free block (not this block, since it's used !)
        if (cursor->prevFree != NULL) {
            ((Metadata *)(cursor->prevFree))->nextFree = (uint8_t *) new_free_meta;
        }

        // Mark block as used
        cursor->blockSize = cursor->blockSize | 0x1;

        // Return the address after the metadata
        return (void *)(cursor + ALIGN(sizeof(Metadata)));     

    } else {

        // If not enough space to split, we just take the whole block
        if (cursor->prevFree != NULL) {
            ((Metadata *)(cursor->prevFree))->nextFree = cursor->nextFree;
        }

        if (cursor->nextFree != NULL) {
            ((Metadata *)(cursor->nextFree))->prevFree = cursor->prevFree;
        }

        return (void *)(cursor + ALIGN(sizeof(Metadata)));
    }
}

void *my_malloc(size_t size) {

    if (size <= 0) {
        return NULL;
    }

    Metadata *cursor = (Metadata *) MY_HEAP;
    int minSize = INT32_MAX;
    uint8_t *address;

    while (cursor != NULL) {

        printf("Found block of size %d, comparing to %d\n", cursor->blockSize, minSize);
        // Find best fit
        if (cursor->blockSize < minSize) {

            minSize = cursor->blockSize;
            address = (uint8_t *) cursor;
        }

        cursor = (Metadata *) cursor->nextFree;
    }

    // Aucun bloc trouvé
    if (address == NULL) { return NULL; }

    printf("Found address! %p\n", address);

    return place_block(address, size);
}

int main(void) {

  init();

  // Test my_malloc
  void *ptr1 = my_malloc(1);
  printf("Adresse allouée 1 : %p\n", ptr1);

  void *ptr2 = my_malloc(17);
  printf("Adresse allouée 2 : %p\n", ptr2);

  void *ptr3 = my_malloc(13);
  printf("Adresse allouée 3 : %p\n", ptr3);

  return 0;
}
