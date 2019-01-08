#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
	int size;
	int **array;
} Square;

Square * construct_square(char *filename);
int verify_magic(Square * square);

int main(int argc, char *argv[])
{
	// Check input arguments to get filename
	if (argc != 2){
		printf("Usage: ./verify_magic <filename>\n");
		exit(1);
	}
	
	// Construct square
	Square *matrix = construct_square(argv[1]);		
	
	// If file can't be open
	if (matrix == NULL){
		printf("Cannot open file for reading.\n");
		exit(1);	 
	}
	// Verify if it's a magic square and print true or false	
	if(verify_magic(matrix)){
		printf("true\n");
	}
	else{
		printf("false\n");
	}
	for (int i =0; i < matrix->size; i++){
		free(*(matrix->array + i));	
	}
	free(matrix->array);
	free(matrix);
	return 0;
}

/* Reads the input file to initialize a square struct
 * from the contents of the file 
 *
 * filename: input file passed in for reading 		
 * returns the square
 */
Square * construct_square(char *filename)
{
	int bufferSize = 0;
	
	// Open and read the file
	FILE *pFile;
	pFile = fopen(filename,"r");
        
	// Check if file exists and read the first line to get the square size
	if(!pFile){
		return NULL;
	}
	else{	
		// Check if file is empty or not by reading first line
		int firstLine = fscanf(pFile, "%d", &bufferSize);
		if (firstLine <= 0){
			fclose(pFile);
			return NULL;
		}			
 	// Initialize a new Square struct of that size
 	Square * instance = malloc(sizeof(Square));
	if (instance == NULL){
		fclose(pFile);
		return NULL;
	}
	// Allocate memory for size and array of square
	instance->size = bufferSize;
	instance->array = malloc(bufferSize * sizeof(int*));
	
	// Free memory if array is null
	if (instance->array == NULL){
		fclose(pFile);
		free(instance);
		return NULL;
	}

	// Create pointers to array index
	for (int i = 0; i < bufferSize; i++ ) {
	    *(instance->array + i) = malloc(bufferSize * sizeof(int));
		// Free memory if index is null
		if (*(instance->array +i) == NULL){
			for (int j = 0; j < i; j++){
				free(*(instance->array + j));
			}
		free(instance->array);
		free(instance);
		return NULL;
		}
	}
	
	// Read the rest of the file to fill up the square
	for(int i = 0; i < bufferSize; i++){
		char buffer[255];
		fscanf(pFile, "%s",buffer);		
		char *token;
		int column= 0;
		token = strtok(buffer, ",");
		while (column < bufferSize){	
			int element = atoi(token);
			*(*(instance->array + i) + column) = element;
			token = strtok(NULL, ",");
			column++;
		}			;
        }	
	fclose(pFile);
 	return instance;
	}
}

/* Verifies if the square is a magic square
 * 
 * square: data of Struct square
 * returns 1(true) or 0(false)
 */
int verify_magic(Square * square)
{	
	// Create array to store the calculated sums
	int *check = malloc((square->size + square->size + 2) * sizeof(int));
	
	// Check all rows sum to same number
	for  (int i = 0; i < square->size; i++){
		int temp = 0;
		for (int j = 0; j < square->size; j++){
			temp += *(*(square->array + i) + j);
		}
		*(check + i) = temp;
	}
			 
	// Check all cols sum to same number
	for  (int i = 0; i < square->size; i++){
		int temp = 0;
		for (int j = 0; j < square->size; j++){
			temp +=  *(*(square->array + j) + i);
		}
		*(check + square->size + i) = temp;
	}

	// Check main diagonal
	int temp = 0;
	for (int i = 0; i < square->size; i++){
		temp +=	*(*((square->array) + i) + i);
	}
		*(check + (2*square->size))= temp;
		
	// Check secondary diagonal
	temp = 0;
	for (int i =0; i < square->size; i++){
		temp += *(*(square->array + i) + (square->size - i - 1));
	}
		*(check + (2*(square->size)) + 1) = temp;
	
	// Check if all the sums equal to each other
	for  (int i = 0; i < (2*(square->size)); i++){
		if (*(check) != *(check + i)){
			free(check);
			return 0;
		}
	}
	free(check);
	return 1;
}

