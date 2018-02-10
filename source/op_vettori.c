/*
Autore: Alessandro Bianchi
Descrizione: contiene una serie di funzionalit� legate all'utilizzo di array
*/
#include <stdio.h>
#include <stdlib.h>
#include "strutture_huffman.h"

/// <summary>
/// Inizializza gli elementi di un array di tipo Nodo**
/// </summary>
/// <param name="nodi">Riferimento all'array</param>
void inizializzaArrayNodi(Nodo **nodi)
{
	for (int i = 0; i < COMB; i++)
	{
		nodi[i] = malloc(sizeof(Nodo));
		nodi[i]->carattere = -2;
		nodi[i]->frequenza = 0;
		nodi[i]->left = NULL;
		nodi[i]->right = NULL;
	}
}

/// <summary>
/// Inizializza gli elementi di un array di tipo Codice**
/// </summary>
/// <param name="codici">Riferimento all'array</param>
void inizializzaArrayCodici(Codice **codici)
{
	for (int i = 0; i < COMB; i++)
	{
		codici[i] = malloc(sizeof(Codice));
		codici[i]->carattere = i;
		codici[i]->numero_bit = 0;
		codici[i]->codice = 1;
		codici[i]->codice_canonico = malloc(sizeof(char)*BITxCARATTERE);

		for (int c = 0; c < BITxCARATTERE; c++)
			codici[i]->codice_canonico[c] = '9';
	}
}

/// <summary>
/// Libera gli elementi di un array di tipo Nodo**
/// </summary>
/// <param name="nodi">Array da liberare</param>
void liberaNodi(Nodo **nodi)
{
	for (int i = 0; i < COMB; i++)
		free(nodi[i]);
}

/// <summary>
/// Libera gli elementi di un array di tipo Codice**
/// </summary>
/// <param name="codici">Array da liberare</param>
void liberaCodici(Codice **codici)
{
	for (int i = 0; i < COMB; i++)
		free(codici[i]);
}