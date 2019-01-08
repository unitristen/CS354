////////////////////////////////////////////////////////////////////////////////
// Main File:        generate_magic.c
// This File:        generate_magic.c
// Other Files:      verify_magic.c
// Semester:         CS 354 Spring 2017
//
// Author:           tristen hallock
// Email:            hallock2@wisc.edu
// CS Login:         tristen
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
	int size;
	int **array;
} Square;

int get_square_size();
Square * generate_magic(int size);
void write_to_file(Square * square, char *filename);

int main(int argc, char *argv[])
{
	// Check input arguments to get filename
	if(argc != 2){
		printf("Usage: .generate_magic <filename>\n");
		exit(1);
	}
	// Get size from user
	int size = get_square_size();
	if(size == 0){
		exit(1);
	}
	// Generate the magic square
	Square * square = generate_magic(size);
	// Write the square to the output file
	write_to_file(square, argv[1]);
	// Free memory
	for(int i = 0; i < square->size; i++){
		free(*(square->array + i));
	}
	free(square->array);
	free(square);
	return 0;
}

/* get_square_size prompts the user for the magic square size
 * checks if it is an odd number >= 3 and returns the number
 */
int get_square_size()
{
	int size;
	printf("Enter size of magic square, must be odd\n");
	scanf("%d", &size);
	if(size % 2 == 0 || size < 3){
		printf("Size must be an odd number >= 3.\n");
		return 0;
	}else{
		return size;
	}
}

/* generate_magic constructs a magic square of size n
 * using the Siamese algorithm and returns the Square struct
 */
Square * generate_magic(int n)
{
	// Initialize new Square struct of size n
	Square * new_square = malloc(sizeof(Square));
	if(new_square == NULL){
		return NULL;
	}
	// Allocate memory for size and array
	new_square->size =  n;
	new_square->array = malloc(n*sizeof(int*));
	// If array is null, then free it
	if(new_square->array == NULL){
		free(new_square);
		return NULL;
	}
	// create array index pointers
	for(int i = 0; i < n; i++){
		*(new_square->array + i) = malloc(n*sizeof(int));
		// If array index is null, then free it
		if(*(new_square->array + i) == NULL){
			for(int j = 0; j < i; j++){
				free(*(new_square->array + j));
			}
			free(new_square->array);
			free(new_square);
			return NULL;
		}
	}
	int num_items = n * n;
	//Start
	int i = 0;
	int j = n/2;
	// Siamese Method
	for(int num = 1; num <= num_items; num++){
		*(*(new_square->array + i) + j) = num;
		i--;
		j++;
		if(num % n == 0){
			i = i + 2;
			j--;
		}else{
			if(j == n)
				j = j - n;
			else if (i < 0)
				i = i + n;
		}
	}
	return new_square;	
}

/* write_to_file opens up a new file(or overwrites the existing file)
 * and writes out the square in the format expected by verify_magic.c
 */
void write_to_file(Square * square, char *filename)
{
	FILE *file;
	file = fopen(filename, "w");
	// write magic square matrix to file
	fprintf(file, "%d\n", square->size);
	for(int i = 0; i < square->size; i++){
		int col = 0;
		while(col < square->size){
			fprintf(file,"%d", *(*(square->array + i) + col));
			if(col < square->size - 1){
				fprintf(file, ",");
			}
			col++;
		}
		fprintf(file, "\n");
	}
	fclose(file);
}	
