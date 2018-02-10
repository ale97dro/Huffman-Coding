/*
Autore: Alessandro Bianchi
Inizio sviluppo: 10/10/2017
Ultimo aggiornamento: 19/01/2017
Descrizione: funzionalit� per la lettura/scrittura di file

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strutture_huffman.h"
#include <math.h>


char buffer[BUFFCOMPR];
int conta_buffer = 0;
int bit_vuoti = 0;
char buffer_scrittura[BUF_SIZE];
int indice_buffer_scrittura = 0;

/// <summary>
/// Esegue la scansione del file, contando la frequenza di apparizione dei caratteri presenti in esso
/// </summary>
/// <param name="input">File da scansionare</param>
/// <param name="array">Array nel quale salvare i caratteri letti e le relative frequenze</param>
/// <param name="elementi_inseriti">Numero che indica quanti caratteri diversi (su 256) sono presenti nel file scansionato</param>
/// <param name="somma_frequenze">Numero totale dei caratteri che compongono il file</param>
void scansioneFile(char *input, Nodo **array, int *elementi_inseriti, int *somma_frequenze)
{
	FILE *infile;
	unsigned int c;
	int inseriti = 0, somma_fr = 0;

	if ((infile = fopen(input, "rb")) != NULL) //apro il file binario
	{
		while ((c = fgetc(infile)) != EOF) //leggo fino a quando non arrivo alla fine
		{
			char presente = FALSE;
			
			
			for (int i = 0; i < inseriti; i++)
			{
				if (array[i]->carattere == c)
				{
					array[i]->frequenza++;
					presente = TRUE;
				}
			}

			if (presente == FALSE)
			{
				array[inseriti]->carattere = c;
				array[inseriti]->frequenza++;
				inseriti++;
			}
			somma_fr++;
		}

		*elementi_inseriti = inseriti;
		*somma_frequenze = somma_fr;

		fclose(infile);
	}
}

/// <summary>
/// Esegue la scansione del file, contando la frequenza di apparizione dei caratteri presenti in esso
/// Utilizza un buffer per rendere pi� veloce la lettura
/// </summary>
/// <param name="input">File da scansionare</param>
/// <param name="array">Array nel quale salvare i caratteri letti e le relative frequenze</param>
/// <param name="elementi_inseriti">Numero che indica quanti caratteri diversi (su 256) sono presenti nel file scansionato</param>
/// <param name="somma_frequenze">Numero totale dei caratteri che compongono il file</param>
void scansioneFileBuffer(char *input, Nodo **array, int *elementi_inseriti, int *somma_frequenze)
{
	FILE *infile;
	int c;
	int n;
	unsigned char buffer[BUF_SIZE]; 
	int inseriti = 0, somma_fr = 0;

	char isPresente = FALSE;

	if ((infile = fopen(input, "rb")) != NULL) //apro il file binario
	{
		do
		{
			n = fread(buffer, sizeof(unsigned char), BUF_SIZE, infile);

			for (int k = 0; k < n; k++)
			{
				array[buffer[k]]->carattere = buffer[k];
				array[buffer[k]]->frequenza++;
				if (array[buffer[k]]->frequenza == 1)
					inseriti++;

				somma_fr++;
				isPresente = FALSE; 
			}
		} while (n == BUF_SIZE);

		*elementi_inseriti = inseriti;
		*somma_frequenze = somma_fr;

		fclose(infile);
	}
}

/// <summary>
/// Inizializza il buffer di conversione byte a 0
/// </summary>
void inizializzaBuffer()
{
	for (int i = 0; i < BUFFCOMPR; i++)
	{
		buffer[i] = 0;
	}
}

/// <summary>
/// Esegue la conversione del buffer di compressione da binario a decimale
/// </summary>
int conversioneBinarioDecimale()
{
	int somma = 0;

	for (int i = 0; i < BUFFCOMPR; i++)
	{
		somma += buffer[i] * pow(2, BUFFCOMPR - i - 1);
	}

	return somma;
}

/// <summary>
/// Svuota il contenuto del buffer di conversione in un buffer di scrittura. Quando quest'ultimo sar� pieno, esso verr� svuotato
/// scrivendo il suo contenuto sul file
/// </summary>
/// <param name="outfile">File su cui svuotare il contenuto del buffer di scrittura</param>
void svuotaBuffer(FILE *outfile)
{
	int somma = 0;

	somma = conversioneBinarioDecimale();

	buffer_scrittura[indice_buffer_scrittura] = somma;
	indice_buffer_scrittura++;

	if (indice_buffer_scrittura == BUF_SIZE)
	{
		fwrite(buffer_scrittura, sizeof(char), BUF_SIZE, outfile);
		indice_buffer_scrittura = 0;
	}

	bit_vuoti = BUFFCOMPR - conta_buffer;
	conta_buffer = 0;
}

/// <summary>
/// Scrivi su file i caratteri codificati con codifica canonica di Huffman che costituiscono il file compresso. 
/// Per poterlo fare, vengono letti i caratteri dal file di input.
/// </summary>
/// <param name="input">Percorso del file di input</param>
/// <param name="output">Percorso del file di output (file compresso)</param>
/// <param name="codici">Array contenente le codifiche canoniche dei caratteri e le loro relative lunghezze</param>
void scriviCompressione(char *input, char *output, Codice **codici)
{
	FILE *infile = fopen(input, "rb");
	FILE *outfile = fopen(output, "wb");
	unsigned char buffer_lettura[BUF_SIZE];
	int n;
	unsigned int c;

	inizializzaBuffer();

	if ((infile != NULL) && (outfile != NULL))
	{
		//scrivo le lunghezze dei caratteri della codifica canonica
		for (int i = 0; i < COMB; i++)
		{
			char numero_bit = codici[i]->numero_bit;
			fputc(numero_bit, outfile);
		}

		do //lettura dei caratteri sul file con buffer
		{
			n = fread(buffer_lettura, sizeof(char), BUF_SIZE, infile);

			for (int k = 0; k < n; k++)
			{
				for (int i = 0; i < codici[buffer_lettura[k]]->numero_bit; i++)
				{
					buffer[conta_buffer] = codici[buffer_lettura[k]]->codice_canonico[BITxCARATTERE - codici[buffer_lettura[k]]->numero_bit + i];
					conta_buffer++;

					if (conta_buffer == BUFFCOMPR)
					{
						svuotaBuffer(outfile);
					}
				}
			}
		} 
		while (n == BUF_SIZE);

		svuotaBuffer(outfile);
		fwrite(buffer_scrittura, sizeof(char), indice_buffer_scrittura, outfile);
		//quello sotto scrive quanti bit sono stati lasciati in bianco
		fputc(bit_vuoti, outfile);
		
		fclose(infile);
		fclose(outfile);
	}
}


//FUNZIONI DECOMPRESSIONE

/// <summary>
/// Ritorna la lunghezza totale (in caratteri) del file dopo aver letto le lunghezze dei caratteri codificati
/// </summary>
/// <param name="input">File da cui leggere le lunghezze</param>
/// <param name="codici">Array contenente le lunghezze delle codifiche lette dal file</param>
/// <returns>Caratteri letti</returns>
int leggiLunghezzeCaratteri(char *input, Codice **codici)
{
	FILE *infile = fopen(input, "rb");
	int conta_letti = 0;
	int c;

	if (input != NULL)
	{
		while (((c = fgetc(infile)) != EOF) && (conta_letti < 256))
		{
			codici[conta_letti]->numero_bit = c;

			conta_letti++;
		}

		fclose(infile);
	}

	return conta_letti;
}

/// <summary>
/// Converte un numero decimale nel corrispettivo binario.
/// Il risultato viene salvato nel buffer di conversione
/// </summary>
/// <param name="numero">Numero da convertire</param>
void conversioneDecimaleBinario(int numero)
{
	//nello stesso ordine in cui mi arriva
	int temp;
	inizializzaBuffer();
	conta_buffer = 7;

	while (numero != 0)
	{
		buffer[conta_buffer] = numero % 2;
		numero = numero / 2;
		conta_buffer--;
	}
}

/// <summary>
/// Ritorna il nodo figlio della radice a seconda del carattere passato:
///		0: figlio sinistro
///		1: figlio destro
/// </summary>
/// <param name="root">Radice dell'albero (o sotto-albero)</param>
/// <param name="carattere">Carattere da utilizzare per stabilere il figlio da ritornare.</param>
/// <returns>Nodo figlio</returns>
Nodo* ritornaNodoAlbero(Nodo *root, char carattere)
{
	if (carattere == 0)
	{
		return root->left;
	}
	else //carattere==1
	{
		return root->right;
	}
}

/// <summary>
/// Scrivi su file i caratteri decodificati con codifica canonica di Huffman che costituiscono il file decompresso. 
/// Per poterlo fare, vengono letti i caratteri dal file di input (in questo caso, il file precedentemente compresso).
/// </summary>
/// <param name="input">Percorso del file di input (file compresso)</param>
/// <param name="output">Percorso del file di output (file decompresso)</param>
/// <param name="root">Root dell'albero di decodifica costruito attraverso la codifica canonica</param>
/// <param name="caratteri_letti">Numero dei caratteri letti</param>
void decomprimiFile(char *input, char *output, Nodo *root, int caratteri_letti)
{
	FILE *infile = fopen(input, "rb");
	FILE *outfile = fopen(output, "wb");

	int lunghezza_file;
	int c;
	int char_decodificati[256];
	int conta_decodificati = 0;

	if ((infile != NULL) && (outfile != NULL))
	{
		fseek(infile, 0, SEEK_END);
		lunghezza_file = ftell(infile);
		rewind(infile);

		fseek(infile, 256 * sizeof(char), SEEK_SET);

		Nodo *carattere = root;
		conta_buffer = 8;

		while (caratteri_letti < (lunghezza_file-2))
		{
			c = fgetc(infile);
			conversioneDecimaleBinario(c);
			caratteri_letti++;

			conta_buffer = 0;

			do
			{
				carattere = ritornaNodoAlbero(carattere, buffer[conta_buffer]);

				if (carattere->carattere != -1)
				{
					fputc(carattere->carattere, outfile);
					carattere = root;
				}

				conta_buffer++;
			} 
			while (conta_buffer < BUFFCOMPR);
		}

		//Gestione dell'ultimo carattere
		c = fgetc(infile);
		conversioneDecimaleBinario(c);
		int ultimo_carattere = fgetc(infile); //sono i bit vuoti! non usati
		int indice = 8;
		conta_buffer = ultimo_carattere;
		
		while (indice > ultimo_carattere)
		{
			carattere = ritornaNodoAlbero(carattere, buffer[conta_buffer]);

			if (carattere->carattere != -1)
			{
				fputc(carattere->carattere, outfile);
				carattere = root;
			}
			conta_buffer++;
			indice--;
		}

		fclose(infile);
		fclose(outfile);
	}
}

