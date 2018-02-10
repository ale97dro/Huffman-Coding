#include "strutture_huffman.h"

void scansioneFile(char *input, Nodo **array, int *elementi_inseriti, int *somma_frequenze);
void scansioneFileBuffer(char *input, Nodo **array, int *elementi_inseriti, int *somma_frequenze);
void scriviCompressione(char *input, char *output, Codice **codici);
//FUNZIONI PER DECOMPRESSIONE
int leggiLunghezzeCaratteri(char *input, Codice **codici);
void decomprimiFile(char *input, char *output, Nodo *root, int caratteri_letti);

