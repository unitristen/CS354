////////////////////////////////////////////////////////////////////////////////
// Main File:        verify_magic.c
// This File:        verify_magic.c
// Other Files:      generate_magic.c
// Semester:         CS 354 Spring 2017
//
// Author:           Tristen Hallock
// Email:            hallock2@wisc.edu
// CS Login:         tristen.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
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
	if(argc != 2){
		printf("Usage: ./verify_magic <filename>\n");
		exit(1);
	}
	// Construct square
	Square * main_square = construct_square(argv[1]);
	
	// if file doesn't open properly
	if (main_square == NULL){
		printf("Cannot open file for reading.\n");
		exit(1);
	}
	// Verify if it's a magic square and print true or false
	if(verify_magic(main_square)){
		printf("true\n");
	}else{
		printf("false\n");
	}
	// free memory occupied by the square
	for(int row = 0; row < main_square->size; row++){
		free(*(main_square->array + row));
	}
	free(main_square->array);
	free(main_square);
	return 0;
}

/* construct_square reads the input file to initialize a square struct
 * from the contents of the file and returns the square.
 * The format of the file is defined in the assignment specifications
 */
Square * construct_square(char *filename)
{
	int square_size = 0;
	// Open and read the file
	FILE *file;
	file = fopen(filename, "r");
	// Read the first line to get the square size
	if(!file){
		return NULL;
	}else{
//		int square_size;
		int is_valid_input = fscanf(file, "%d", &square_size);
		if(is_valid_input <= 0){
			fclose(file);
			return NULL;
		}
 		
	// Initialize a new Square struct of that size
	Square * new_square = malloc(sizeof(Square));
	if (new_square == NULL){
		fclose(file);
		return NULL;
	}
	//int num_items = square_size * square_size;
	// allocate memory for size and array of square
	new_square->size = square_size;
	new_square->array = malloc(square_size*sizeof(int*));
	
	// free momory if array is null
	if(new_square -> array == NULL){
		fclose(file);
		free(new_square);
		return NULL;
	}
	// create pointers for rows in square->array, and allocate memory for them
	for(int row = 0; row < square_size; row++){
		*(new_square->array + row) = malloc(square_size * sizeof(int));
		// if the index of the created pointer is null then free its memory
		if(*(new_square->array+row) == NULL){
			for(int j = 0; j < row; j++){
				free(*(new_square-> array + j));
			}
			free(new_square->array);
			free(new_square);
			return NULL;
		}
	}
	// Read the rest of the file to fill up the square
	for(int row = 0; row < square_size; row++){
		char raw_values[255];
		fscanf(file, "%s", raw_values);
		char *token;
		int col = 0;
		token = strtok(raw_values, ",");
		//walk through the tokens in this row
		while(col < square_size){
			int value = atoi(token);
			*(*(new_square->array + row) + col) = value;	
			token = strtok(NULL, ","); // continue parsing from last spot in raw_values
			col++;
		}
	}
	fclose(file);
	return new_square;

	}
}


/* verify_magic verifies if the square is a magic square
 * 
 * returns 1(true) or 0(false)
 */
int verify_magic(Square * square)
{
	// create an array to store computed sums and allocate memory for it
	int * sums = malloc((square->size + square->size + 2)*sizeof(int));
	// Check all rows sum to same number
	for(int row = 0; row < square->size; row++){
		int tmp = 0;
		for(int col = 0; col < square->size; col++){
			tmp = tmp + *(*(square->array + row)+col);
		}
		*(sums + row) = tmp;
	}
	// Check all cols sum to same number
	for(int row = 0; row < square->size; row++){
		int tmp = 0;
		for(int col = 0; col < square->size; col++){
			tmp = tmp + *(*(square->array + col) + row);
		}
	*(sums + square->size + row) = tmp;
	}
	// Check main diagonal
	int tmp = 0;
	for(int row = 0; row < square->size; row++){
		tmp = tmp + *(*((square->array) + row) + row);
	}
	*(sums + (2*square->size)) = tmp;
	// Check secondary diagonal
	tmp = 0;
	for(int row = 0; row < square->size; row++){
		tmp = tmp + *(*(square->array + row) + (square->size - row - 1));
	}
	*(sums + 2*(square->size) + 1) = tmp;
	// Check to see if the sums are equal to each other
	for(int i = 0; i < 2*(square->size); i++){
		if(*(sums) != *(sums + i)){
			free(sums);
			return 0;
		}
	}
	free(sums);
	return 1;
}
