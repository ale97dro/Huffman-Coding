/*
Autore: Alessandro Bianchi
Descrizione: funzionalit� per compressione e decompressione Huffman

Nome: Mercedes
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strutture_huffman.h"
#include "file.h"
#include "op_vettori.h"
#include "huffman.h"

/// <summary>
/// Ordina gli elementi di un array di tipo Codice** in base all'attributo numero_bit. L'algoritmo � di tipo Bubble Sort.
/// E' necessario che l'algoritmo di ordinamento sia stabile.
/// </summary>
/// <param name="nodi">Riferimento all'array</param>
void ordinaCodici(Codice **codici)
{
	Codice *temp;

	for (int c = 0; c < (COMB - 1); c++)
	{
		for (int d = 0; d < (COMB - c - 1); d++)
		{
			if (codici[d]->numero_bit > codici[d + 1]->numero_bit)
			{
				temp = codici[d];
				codici[d] = codici[d + 1];
				codici[d + 1] = temp;
			}
		
		}
	}
}

/// <summary>
/// Funzione delegata per ordinamento qsort()
/// Ordina i nodi in maniera decrescente in base alla loro frequenza
/// </summary>
/// <param name="p1">Primo nodo del confronto</param>
/// <param name="p2">Secondo nodo del confronto</param>
/// <returns>Differenza tra i termini del confronto</returns>
int compareNodi(const void *p1, const void *p2)
{
	const Nodo *nodo1 = *(const Nodo **)p1;
	const Nodo *nodo2 = *(const Nodo **)p2;

	return nodo2->frequenza - nodo1->frequenza;
}

/// <summary>
/// Funzione delegata per ordinamento qsort()
/// Ordina i nodi in maniera crescente in base al loro codice ASCII
/// </summary>
/// <param name="p1">Primo nodo del confronto</param>
/// <param name="p2">Secondo nodo del confronto</param>
/// <returns>Differenza tra i termini del confronto</returns>
int compareCodiciAlfabetico(const void *p1, const void *p2)
{
	const Codice *codice1 = *(const Codice**)p1;
	const Codice *codice2 = *(const Codice**)p2;

	return codice1->carattere - codice2->carattere;
}

/// <summary>
/// Ordina gli elementi di un array di tipo Nodo** in base all'attributo frequenza.
/// Nell'utilizzo corrente, questo algoritmo di ordinamento deve ordinare solamente un nuovo elemento alla volta
/// </summary>
/// <param name="nodi">Riferimento all'array</param>
/// <param name="limite">Numero di elementi nell'array</param>
void insertionSort(Nodo **nodi, int limite)
{
	int j;
	Nodo *temp;

	for (int i = 1; i < limite; i++)
	{
		temp = nodi[i];
		j = i - 1;
		while ((j >= 0) && (nodi[j]->frequenza < temp->frequenza))
		{
			nodi[j + 1] = nodi[j];
			j--;
		}
		nodi[j + 1] = temp;
	}
}

/// <summary>
/// Costrutisci l'albero per la codifica di Huffman partendo da un array di nodi contenenti le informazioni sui caratteri da codificare
/// </summary>
/// <param name="nodi">Riferimento all'array</param>
/// <param name="elementi_inseriti">Numero di caratteri che compaiono nel file da cui partire per la costruzione dell'albero di codifica</param>
void costruisciAlbero(Nodo **nodi, int elementi_inseriti)
{
	int nodi_rimanenti = elementi_inseriti; //=256
	int i = 0; //solo per controllo

	while (nodi_rimanenti > 1) //nodi_rimanenti=256
	{
		Nodo *nuovo = (Nodo*)malloc(sizeof(Nodo)); //Nuovo nodo dell'albero

		nuovo->carattere = -1; //Valore "fasullo": la fusione di due nodi non corrisponde ad un carattere
		nuovo->frequenza = nodi[nodi_rimanenti - 1]->frequenza + nodi[nodi_rimanenti - 2]->frequenza; //Somma della frequenze dei due nodi che devono essere fusi
		nuovo->left = nodi[nodi_rimanenti-1]; //Riferimento al figlio sinistro
		nuovo->right = nodi[nodi_rimanenti - 2]; //Riferimento al figlio destro
		nodi[nodi_rimanenti - 2] = nuovo; //Assegno il nuovo nodo nell'albero
		
		nodi_rimanenti--;
		i++; //solo controllo

		insertionSort(nodi, nodi_rimanenti); //Riordino il l'array dei nodi
	}
}

/// <summary>
/// Costruisci la tabella dei codici di Huffman partendo dall'albero di codifica costruito precedentemente
/// Adattato da fillTable(...) https://www.programminglogic.com/implementing-huffman-coding-in-c/
/// </summary>
/// <param name="root">Root dell'albero di codifica</param>
/// <param name="codici">Riferimento all'array di codici</param>
/// <param name="codice">Array di interi che ospita temporaneamente la codifica di un carattere</param>
/// <param name="indice">Valore che indica il numero di elementi inseriti in codice</param>
void costruisciTabellaCodici(Nodo *root, Codice **codici, int indice)
{
	if (root->carattere != -1) //sono arrivato al carattere
	{
		codici[root->carattere]->numero_bit = indice + 1; //metto la lunghezza del codice
	}
	else
	{
		costruisciTabellaCodici(root->left, codici, (indice+1)); //richiamo la funzione ricorsivamente (andando a sinistra)
		costruisciTabellaCodici(root->right, codici, (indice + 1)); //richiamo la funzione ricorsivamente (andando a destra)
	}
}

/// <summary>
/// Somma 1 bit ad un numero binario gestito tramite un array di char
/// </summary>
/// <param name="codice">Riferimento all'array di caratteri a cui bisogna sommare 1</param>
void somma1Bit(char *codice)
{
	int c = BITxCARATTERE - 1;

	while ((c >= 0) && (codice[c] == 1))
	{
		codice[c] = 0;
		c--;
	}
	codice[c] = 1;
}

/// <summary>
/// Shifta le posizioni di un vettore di char
/// </summary>
/// <param name="codice">Riferimento all'array di caratteri di cui bisogna eseguire lo shift</param>
/// <param name="diff_lung">Valore che rappresenta la differenza di lunghezza tra il carattere attuale e il precedente</param>
void shiftVettore(char *codice, int diff_lung)
{
	//shifto a sinistra di codici[i]->numero_bit - lunghezza_precedente
	char temp[BITxCARATTERE];
	//copia dell'array codice
	for (int c = 0; c < BITxCARATTERE; c++)
		temp[c] = codice[c];

	int k = BITxCARATTERE - 1;

	for (int c = 0; c < diff_lung; c++)
	{
		codice[BITxCARATTERE - c - 1] = 0;
		k--;
	}

	for (int c = BITxCARATTERE - 1; c >= 0; c--)
	{
		if (k>0)
			codice[k] = temp[c];
		k--;
	}
}

/// <summary>
/// Realizza la codifica canonica di Huffman sfruttando le funzioni somma1Bit(...) e shiftVettore(...)
/// </summary>
/// <param name="codici">Riferimento all'array di caratteri e relativa codifica</param>
void codificaCanonica(Codice **codici)
{
	int lunghezza_precedente = 0; //lunghezza del codice precedente
	char codice[BITxCARATTERE];
	for (int i = 0; i < BITxCARATTERE; i++)
		codice[i] = 0;

	//int canonizzati = 0; //numero di elementi canonizzati (variabile di debug)
	char primo = TRUE;

	for (int i = 0; i < COMB; i++)
	{
		if ((codici[i]->codice != -1) && (codici[i]->numero_bit > 0))
		{
			if (primo == TRUE)
			{
				primo = FALSE;
			}
			else
			{
				if (codici[i]->numero_bit != lunghezza_precedente) //la lunghezza del codice � diversa da quello precedente: sommo uno e shifto a sinistra
				{
					somma1Bit(codice);
					shiftVettore(codice, (codici[i]->numero_bit - lunghezza_precedente));
				}
				else //caso di lunghezze uguali, somma solo 1 al codice senza shiftare il vettore
				{
					somma1Bit(codice);
				}
			}
			
			//dopo aver modificato codice, lo assegno al corrispondente
			for (int c = 0; c < BITxCARATTERE; c++)
				codici[i]->codice_canonico[c] = codice[c];

			lunghezza_precedente = codici[i]->numero_bit;
		}
	}
}

/// <summary>
/// Comprimi un file utilizzando l'algoritmo di Huffman
/// </summary>
/// <param name="input">Percorso del file da comprimere</param>
/// <param name="output">Percorso nel quale verr� salvato il file compresso</param>
void huffman(char *input, char *output)
{	
	//Dichiarazioni
	Nodo **nodi;
	Nodo *root;
	Codice **codici;
	int elementi_inseriti, somma_frequenze;
	
	//Inizializzazioni
	nodi = malloc(COMB * sizeof(Nodo*));
	codici = malloc(COMB * sizeof(Codice*));
	elementi_inseriti = 0;
	somma_frequenze = 0;
	inizializzaArrayNodi(nodi); //inizializzo array di nodi
	inizializzaArrayCodici(codici);

	//Codifica di Huffman vera e propria
	scansioneFileBuffer(input, nodi, &elementi_inseriti, &somma_frequenze);
	qsort(nodi, COMB, sizeof(Nodo**), compareNodi); //riordina vettore delle frequenze
	costruisciAlbero(nodi, elementi_inseriti); //costruisco l'albero delle frequenze
	root = nodi[0]; //Nodo[0] dove si trova la radice dell'albero

	if (root->carattere != -2) //se root->carattere==-2 -> albero vuoto!
	{
		if (root->left == NULL && root->right == NULL) //se c'� solo un carattere e sta nella root
			costruisciTabellaCodici(root, codici, 0);
		else
			costruisciTabellaCodici(root, codici, -1); //se ci sono almeno due caratteri

		ordinaCodici(codici); //algoritmo stabile (richiesto da Huffman canonico)
		codificaCanonica(codici);
		qsort(codici, COMB, sizeof(Codice **), compareCodiciAlfabetico);
	}
	
	//Scrittura su file: funzione che riempe un buffer di char (corrispondente a dei bit) globale: quando arriva a 8 (oppure un suo multiplo) converto il contenuto in un int e poi lo scrivo sul file. Dopo ricomincio a scrivere il buffer
	scriviCompressione(input, output, codici);

	//Free delle strutture
	liberaNodi(nodi);
	liberaCodici(codici);
	free(nodi);
	free(codici);
}

/// <summary>
/// Aggiunge un nodo all'albero di decodifica.
/// E' una funzione ricorsiva.
/// </summary>
/// <param name="codice">Array contenente il codice di un carattere</param>
/// <param name="fatti">Numero di bit della codifica canonica del carattere gi� analizzati</param>
/// <param name="root">Root dell'albero di decodifica (o di un suo sotto-albero)</param>
void aggiungiNodo(Codice *codice, int fatti, Nodo *root)
{
	if (codice->numero_bit == 0)
		return;

	if (codice->codice_canonico[BITxCARATTERE - codice->numero_bit + fatti] == 1) //vado nel ramo 1 (destra)
	{
		if (root->right == NULL)
		{
			root->right = (Nodo*)malloc(sizeof(Nodo));
			root->right->frequenza = 1;
			root->right->left = NULL;
			root->right->right = NULL;
			
			if (fatti != (codice->numero_bit-1))
			{
				root->right->carattere = -1;
				aggiungiNodo(codice, (fatti + 1), root->right);
			}
			else
			{
				root->right->carattere = codice->carattere;
			}
		}
		else
		{
			aggiungiNodo(codice, (fatti + 1), root->right);
		}
	}
	else
	{
		if (codice->codice_canonico[BITxCARATTERE - codice->numero_bit + fatti] == 0)
		{
			if (root->left == NULL)
			{
				root->left = (Nodo*)malloc(sizeof(Nodo));
				root->left->frequenza = 0;
				root->left->left = NULL;
				root->left->right = NULL;

				if (fatti != (codice->numero_bit-1))
				{
					root->left->carattere = -1;
					aggiungiNodo(codice, (fatti + 1), root->left);
				}
				else
				{
					root->left->carattere = codice->carattere;
				}
			}
			else
			{
				aggiungiNodo(codice, (fatti + 1), root->left);
			}
		}
	}
}

//   0 <- root -> 1
/// <summary>
/// Costruisce l'albero di decodifica partendo dalle lunghezze della codifica canonica.
/// </summary>
/// <param name="codici">Array contenente le informazioni sull codifica</param>
/// <param name="root">Root dell'albero di decodifica</param>
/// <param name="caratteri_letti">Caratteri presenti nel file da decomprimere</param>
/// <param name="input">Percorso nel quale verr� salvato il file decompresso</param>
/// <param name="output">Percorso nel quale verr� salvato il file decompresso</param>
void ricostruisciAlbero(Codice **codici, Nodo *root, int caratteri_letti, char *input, char *output)
{
	root->carattere = -1;
	root->left = NULL;
	root->right = NULL;

	for (int i = 0; i < COMB; i++)
	{
		if (codici[i]->numero_bit >= 0)
			aggiungiNodo(codici[i], 0, root);
	}

	Nodo *temp = root;
	
	decomprimiFile(input, output, root, caratteri_letti);
}


/// <summary>
/// Decomprime un file utilizzando l'algoritmo di Huffman
/// </summary>
/// <param name="input">Percorso del file da decomprimere</param>
/// <param name="output">Percorso nel quale verr� salvato il file decompresso</param>
void dehuffman(char *input, char *output)
{	
	Codice **codici = malloc(COMB * sizeof(Codice*));
	Nodo *root = malloc(sizeof(Nodo*));
	
	inizializzaArrayCodici(codici);
	int caratteri_letti = leggiLunghezzeCaratteri(input, codici);
	ordinaCodici(codici);
	codificaCanonica(codici);
	qsort(codici, COMB, sizeof(Codice **), compareCodiciAlfabetico);
	ricostruisciAlbero(codici, root, caratteri_letti, input, output);

	//liberaCodici(codici); //problemi su macchina Linux
	//free(root); //con alcuni file non riesce a liberare la memoria (probabilemte cerca di deallocare anche quando non c'è niente di allocato)
	//free(codici); //problemi su macchina Linux
}