#include <stdio.h>
#include <stdint.h> 

#define HEAP_SIZE 64000
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

uint8_t MY_HEAP[HEAP_SIZE];

typedef struct {
    int blockSize;
    uint8_t *nextFree;
    uint8_t *prevFree;
} Metadata;

// Déclaration globale du pointeur vers le premier bloc libre
uint8_t *free_list_head = NULL;

void init() {
    Metadata* first_block = (Metadata *)MY_HEAP; 

    // infos du first_block (on aligne la taille disponible)
    first_block->blockSize = HEAP_SIZE - ALIGN(sizeof(Metadata));
    first_block->nextFree = NULL; 
    first_block->prevFree = NULL; 
    
    free_list_head = MY_HEAP;
}

// ATTENTION : La fonction doit retourner void* et non void
void* my_malloc(size_t size) {
    if (size == 0) {
        return NULL; 
    }

    // Alignement 
    size_t aligned_size = ALIGN(size);

    // Parcourt la liste 
    uint8_t *current_pointer = free_list_head; 

    while (current_pointer != NULL) {
        Metadata *current_meta = (Metadata *)current_pointer; 

        // Le bloc est assez grand ? 
        if (current_meta->blockSize >= aligned_size) {

            // On découpe s'il reste assez de place pour un nouveau bloc complet
            size_t total_block_size = ALIGN(sizeof(Metadata)) + aligned_size; 
            
            if (current_meta->blockSize >= total_block_size + ALIGN(sizeof(Metadata))) {
                // Je crée un nouveau bloc libre pour le reste 
                uint8_t *new_free_pointer = current_pointer + total_block_size; 
                Metadata *new_free_meta = (Metadata *)new_free_pointer; 

                new_free_meta->blockSize = current_meta->blockSize - total_block_size; 
                new_free_meta->nextFree = current_meta->nextFree; 
                new_free_meta->prevFree = current_meta->prevFree; 

                // Mettre à jour les liens de la liste libre 
                if (new_free_meta->nextFree != NULL) {
                    ((Metadata *)(new_free_meta->nextFree))->prevFree = new_free_pointer;
                }

                current_meta->blockSize = aligned_size; 
                current_meta->nextFree = new_free_pointer;

                // Retire le bloc de la liste libre (remplacement par le nouveau bloc découpé)
                if (current_meta->prevFree != NULL) {
                    ((Metadata *)(current_meta->prevFree))->nextFree = new_free_pointer;
                } else {
                    // C'était la tête de liste
                    free_list_head = new_free_pointer;
                }

                if (new_free_meta->nextFree != NULL) {
                    ((Metadata *)(new_free_meta->nextFree))->prevFree = new_free_pointer;
                }

                // Retourne l'adresse utile
                return (void *)(current_pointer + ALIGN(sizeof(Metadata)));
            } else {
                // Pas assez de place pour découper, on prend tout le bloc
                if (current_meta->prevFree != NULL) {
                    ((Metadata *)(current_meta->prevFree))->nextFree = current_meta->nextFree;
                } else {
                    free_list_head = current_meta->nextFree;
                }
                if (current_meta->nextFree != NULL) {
                    ((Metadata *)(current_meta->nextFree))->prevFree = current_meta->prevFree;
                }

                return (void *)(current_pointer + ALIGN(sizeof(Metadata)));
            }
        }

        current_pointer = current_meta->nextFree; 
    }

    // Aucun bloc trouvé
    return NULL; 
}

int main(void) {
    // TEST alignement 
    for (int i = 0; i <= 32; i += 2) {
        printf("ALIGN(%d) = %d\n", i, (int)ALIGN(i)); 
    }

    init();
    
    // Test my_malloc
    void *ptr1 = my_malloc(100);
    printf("Adresse allouée 1 : %p\n", ptr1);

    void *ptr2 = my_malloc(200);
    printf("Adresse allouée 2 : %p\n", ptr2);

    return 0;
}