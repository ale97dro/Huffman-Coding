This is a compression and decompression algorithm made by using the Huffman coding.

It's based on ASCII code.

Here some instrusction to use it:

- If you want to build the project, use the file in source folder and compile them in this way:
				gcc huffman_coding.c huffman.c file.c op_vettori.c -o exeName
- If you want use the .exe file, you have to launch it from command-line with this syntax:
				compression operation: exeName.exe -c inputFile outputFile
				decompression operation: exeName.exe -d inputFile outputFile
	If you use the default .exe, his name his huffmanCoding.exe