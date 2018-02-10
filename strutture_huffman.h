/*
Autore: Alessandro Bianchi
Descrizione: Strutture e costanti utilizzate dall'algoritmo di Huffman
*/

#ifndef STRUTTURE_H
#define STRUTTURE_H

//Costanti booleane
#define TRUE 0
#define FALSE 1

//Costanti numeriche
#define COMB 256 //Numero di caratteri supportati dalla codifica (ASCII=256)
#define BITxCARATTERE 256 //deve essere 256
#define BUF_SIZE 81920
#define BUFFCOMPR 8 //Buffer per la scrittura del file compresso

typedef struct nodo
{
	int carattere;
	int frequenza;
	struct nodo *left, *right;
}Nodo;

typedef struct codice
{
	int carattere;
	unsigned short numero_bit;
	int codice;
	char *codice_canonico; 
}Codice;

#endif