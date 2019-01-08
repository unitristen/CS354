
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
