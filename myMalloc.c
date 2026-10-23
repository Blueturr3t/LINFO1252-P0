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
    // Create "header" which always points to first free block
    Metadata *header = (Metadata *) MY_HEAP;
    size_t header_size = ALIGN(sizeof(Metadata));

    header->blockSize = header_size | 0x1; // Set last bit to 1 to mark as private
    header->nextFree = NULL;
    header->prevFree = NULL;
    
    // Create first free block
    Metadata *first_free = (Metadata *)(MY_HEAP + header_size);
    first_free->blockSize = HEAP_SIZE - header_size;
    first_free->nextFree = NULL;
    first_free->prevFree = (uint8_t *) header; 

    header->nextFree = (uint8_t *) first_free;
}

static void *place_block(uint8_t *address, size_t size) {
    // Set total size with padding
    size_t total_block_size = ALIGN(sizeof(Metadata)) + ALIGN(size);

    Metadata *cursor = (Metadata *) address;
    size_t cursor_size = cursor->blockSize & ~0x1;

    if (cursor_size > total_block_size) { // If we have too much space, split memory in two

        // Create metadata for next free block
        Metadata *new_free_meta = (Metadata *)((uint8_t *)cursor + total_block_size);

        new_free_meta->blockSize = cursor_size - total_block_size;
        new_free_meta->nextFree = cursor->nextFree;
        new_free_meta->prevFree = cursor->prevFree;

        printf("Successfully created new block at address %p of size %d, with nextFree %p and prevFree %p\n", 
               (void*)new_free_meta, new_free_meta->blockSize, (void*)new_free_meta->nextFree, (void*)new_free_meta->prevFree); 

        // Set next block's previous to next free block
        if (new_free_meta->nextFree != NULL) {
            ((Metadata *)(new_free_meta->nextFree))->prevFree = (uint8_t *) new_free_meta;
        }

        // Set prev block's next to next free block
        if (new_free_meta->prevFree != NULL) {
            ((Metadata *)(new_free_meta->prevFree))->nextFree = (uint8_t *) new_free_meta;
        }

        // Mark block as used
        cursor->blockSize = total_block_size | 0x1;

        // Return the address after the metadata 
        return (void *)((uint8_t *)cursor + ALIGN(sizeof(Metadata)));     

    } else {
        // If not enough space to split, we just take the whole block
        if (cursor->prevFree != NULL) {
            ((Metadata *)(cursor->prevFree))->nextFree = cursor->nextFree;
        }

        if (cursor->nextFree != NULL) {
            ((Metadata *)(cursor->nextFree))->prevFree = cursor->prevFree;
        }

        cursor->blockSize = cursor_size | 0x1;

        return (void *)((uint8_t *)cursor + ALIGN(sizeof(Metadata)));
    }
}

void *my_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    Metadata *cursor = (Metadata *) MY_HEAP;
    cursor = (Metadata *) cursor->nextFree; // Cursor is initially header, we go to first free block
    
    int minSize = INT32_MAX;
    Metadata *best_block = NULL;

    while (cursor != NULL) {
        size_t corrected_size = cursor->blockSize & ~0x1;
        printf("Block in %p (of size: %zu), size required : %zu\n", (void*)cursor, corrected_size, ALIGN(sizeof(Metadata)) + ALIGN(size));
        
        if (corrected_size >= (ALIGN(sizeof(Metadata)) + ALIGN(size)) && ((int)corrected_size < minSize)) {
            minSize = (int)corrected_size;
            best_block = cursor;
        }

        cursor = (Metadata *) cursor->nextFree;
    }

    // No block found
    if (best_block == NULL) { 
        printf("Aucun bloc trouvé !\n");
        return NULL; 
    }

    printf("\nFound address! %p\n", (void*)best_block);

    return place_block((uint8_t *)best_block, size);
}

int main(void) {
    init();

    // Test my_malloc
    void *ptr1 = my_malloc(1);
    printf("Adresse allouée 1 : %p\n\n", ptr1);

    void *ptr2 = my_malloc(17);
    printf("Adresse allouée 2 : %p\n\n", ptr2);

    void *ptr3 = my_malloc(13);
    printf("Adresse allouée 3 : %p\n\n", ptr3);

    return 0;
}