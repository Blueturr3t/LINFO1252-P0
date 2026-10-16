# LINFO1252-P0
Réimplémentation de malloc et de free en C

## Stratégie
Facteur d'alignement de 8 octets
Méthode de liste externe et adress_ordered (?)
Metadata in 
```c

typedef struct metadata {

    // disponibilité sur bit de poids faible
    // taille du bloc 
    // référence vers le prochain bloc
    // référence vers le bloc précédent
} Metadata
```
