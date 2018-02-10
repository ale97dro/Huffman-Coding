#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "huffman.h"

int stabilisciOperazione(char *operazione)
{
	if (strcmp(operazione, "-c") == 0) //esegui compressione huffman
		return 0;
	if (strcmp(operazione, "-d") == 0) //esegui decompressione huffman
		return 1;

	return -1;
}


int main(int argc, char *argv[])
{
	char *operazione;
	char *input, *output;

	if (argc != 4)
	{
		printf("Errore nei parametri\n");
		return 0;
	}

	operazione = argv[1];
	input = argv[2];
	output = argv[3];
	

	printf("Compressione del file: %s\n", input);
	
	clock_t inizio = clock();
	FILE* inputStream;
	FILE* tmpStream;
	FILE* outputStream;

	switch (stabilisciOperazione(operazione))
	{
		case 0:
			printf("Compressione Huffman\n");
			huffman(input, output);
			break;
		case 1:
			printf("Decompressione Huffman\n");
			dehuffman(input, output);
			break;
		default:
			printf("Errore nella scelta dell'operazione!");
			break;
	}
	printf("Percorso file compresso: %s\n", output);

	clock_t fine = clock();

	double tempo_passato = (double)(fine - inizio) / CLOCKS_PER_SEC;

	printf("Tempo per l'operazione: %lf\n", tempo_passato);

	printf("\n");
	return 0;
}