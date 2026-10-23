/*
Problemes : savoir ou se trouve la metadata 
partir a la recherche ET gerer la memoire libre 
faire les allocations 

!!! On doit avoir un nombre d'alignement multiple de deux pour pouvoir regarder le dernier bit sinon free c'est pas pratique 
*/

#include <stdio.h>
#include <stdint.h> 

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))


int main(void) {

    // TEST alignement 
    for (int i = 0; i <= 32; i += 2) {
        printf("ALIGN(%d) = %d\n", i, (int)ALIGN(i)); 
    }

    return 0;
}