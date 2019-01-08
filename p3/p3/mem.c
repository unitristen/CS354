///////////////////////////////////////////////////////////////////////////////
// Main File:	mem.c
// This File:	mem.c
// Semester:	CS 354 Spring 2017
// 
// Author:	Tristen Hallock
// Email:	hallock2@wisc.edu
// CS Login:	tristen
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block
 * It also serves as the footer for each free block
 * The blocks are ordered in the increasing order of addresses
 */

typedef struct block_tag{ 
	int size_status;
	
	/*
	 * Size of the block is always a multiple of 4
	 * => last two bits are always zero - can be used to store other information
	 *
	 * LSB -> Least Significant Bit (Last Bit)
	 * SLB -> Second Last Bit
	 * LSB = 0 => free block
	 * LSB = 1 => allocated/busy block
	 * SLB = 0 => previous block is free
	 * SLB = 1 => previous block is allocated/busy
	 *
	 * When used as the footer the last two bits should be zero
	 */
	
	/*

	 * Examples:
	 *
	 * For a busy block with a payload of 24 bytes (i.e. 24 bytes data + an additional 4 bytes for header)
	 * Header:
	 * If the previous block is allocated, size_status should be set to 31
	 * If the previous block is free, size_status should be set to 29
	 *
	 * For a free block of size 28 bytes (including 4 bytes for header + 4 bytes for footer)
	 * Header:
	 * If the previous block is allocated, size_status should be set to 30
	 * If the previous block is free, size_status should be set to 28
	 * Footer:
	 * size_status should be 28
	 *
	 */
} block_tag;


/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
block_tag *first_block = NULL;

/* Global variable - Total available memory */
unsigned int total_mem_size = 0;


#define FALSE 0
#define TRUE  1

/***************** GET MEMORY INFO *******************************/
/*
 * Returns the size of the block referred to by the block_tag parameter header
 */
unsigned int get_block_size(block_tag *header) {
	return header->size_status & 0xFFFFFFFC;
}

/*
 * Returns the free status  of the block referred to by the block_tag parameter header
 */
int is_free(block_tag *header) {
	return !(header->size_status & 0x00000001);
}
/*
 * Returns the free status of the before the block referred to by the block_tag parameter header
 */

int is_prev_free(block_tag *header) {
	return !(header->size_status & 0x00000002);
}

/***************** SET MEMORY INFO *******************************/
/*
 * updates free status of block referred to by block_tag parameter tag
 */
void set_memory_free(block_tag *tag, int is_free)
{
	if (is_free) {
		tag->size_status &= 0xFFFFFFFE;
	} else {
		tag->size_status |= 0x00000001;
	}
}
/*
 * updates free status of the block before the block referred to by block_tag parameter tag
 */
void set_prev_free(block_tag *tag, int is_free) {
	if (is_free) {
		tag->size_status &= 0xFFFFFFFD;
	} else {
		tag->size_status |= 0x00000002;
	}
}
/*
 * updates size status of block referred to by block_tag parameter tag
 */
void set_block_size(block_tag *tag, int padded_size) {
	tag->size_status &= 0x00000003;
	tag->size_status |= padded_size;
}

/***************** NAVIGATE THROUGH MEMORY *******************************/
/*
 * returns the header of the block after the block referred to by block_tag parameter tag
 */
block_tag *get_next_header(block_tag *header) {
	int block_size = get_block_size(header);
	void *ptr = (void *)header + block_size;
	if (ptr >= (void *)first_block + total_mem_size) {
		return NULL;
	} else {
		return ptr;
	}
}
/*
 * returns the foooter of the block referred to by block_tag parameter tag
 */
block_tag *get_footer(block_tag *header) {
	return (block_tag *) ((void *) header + get_block_size(header) - 4);
}

/*
 * traverses the memory and finds the first free block large enough to fit a block
 * of size padded_block_size
 */
block_tag *find_best_fit(unsigned int padded_block_size)
{
	block_tag *current = first_block;
	while (current != NULL) {
		int current_free = is_free(current);
		unsigned int block_size   = get_block_size(current);
		
		if (current_free && padded_block_size <= block_size)
		{
			return current;
		}
		current = get_next_header(current);
	}
	return current;
}

/*
 *
 * Function for allocating 'size' bytes
 * Returns address of the payload in the allocated block on success
 * Returns NULL on failure
 * Here is what this function should accomplish
 * - If size is less than equal to 0 - Return NULL
 * - Round up size to a multiple of 4
 * - Traverse the list of blocks and allocate the best free block which can accommodate the requested size
 * - Also, when allocating a block - split it into two blocks when possible
 * Tips: Be careful with pointer arithmetic
 */
void* Mem_Alloc(int size){
	unsigned int padded_block_size = size;
	// if size is not valid return null
	if(padded_block_size <= 0){
		return NULL;
	}
	
	// if the block has valid size, allocate memory for it
	// make the block size a multiple of 4 and add a header and footer
	if(padded_block_size%4 == 0){
		padded_block_size = padded_block_size     + 4;
	}else if(padded_block_size%4 == 1){
		padded_block_size = padded_block_size + 3 + 4;
	}else if(padded_block_size%4 == 2){
		padded_block_size = padded_block_size + 2 + 4;
	}else{
		padded_block_size = padded_block_size + 1 + 4;
	}
	// There may have been an integer overflow in the addition.
	if (padded_block_size < 0) {
		return NULL;
	}
	
	if (padded_block_size < 8) {
		padded_block_size = 8;
	}
	
	block_tag *free_block = find_best_fit(padded_block_size);
	if (free_block == NULL) {
		// if there is not enough free memory allocate memory for created block
		return NULL;
	}
	
	unsigned int size_of_free_block = get_block_size(free_block);
	
	void *return_value = ((void *) free_block) + 4;
	unsigned int remaining_size = size_of_free_block - padded_block_size;
	
	// if the found block doesn't have enought space to create 2 useful free blocks
	if (remaining_size < 8) {
		set_memory_free(free_block, FALSE);
		block_tag* next = get_next_header(free_block);
		if (next != NULL) {
			set_prev_free(next, FALSE);
		}
		return return_value;
	}
	
	
	// split the current block into two regions
	// initialize the returned block
	set_block_size(free_block, padded_block_size);
	set_memory_free(free_block, FALSE);
	
	// initialize the second new block's header
	block_tag *second_new_block_header = (block_tag*) (((void *) free_block) + padded_block_size);
	set_block_size(second_new_block_header, remaining_size);
	set_memory_free(second_new_block_header, TRUE);
	set_prev_free(second_new_block_header, FALSE);
	
	// initialize the second new block's footer
	block_tag *second_new_block_footer = get_footer(second_new_block_header);
	set_block_size(second_new_block_footer, remaining_size);
	set_memory_free(second_new_block_footer, TRUE);
	set_prev_free(second_new_block_footer, FALSE);

	return return_value;
}


/*
 *
 * Function for freeing up a previously allocated block
 *
 * Argument - ptr: Address of the payload of the allocated block to be freed up
 * Returns 0 on success
 * Returns -1 on failure
 * Here is what this function should accomplish
 *
 * - Return -1 if ptr is NULL
 * - Return -1 if ptr is not within the range of memory allocated by Mem_Init()
 * - Return -1 if ptr is not 4 byte aligned
 * - Mark the block as free
 * - Coalesce if one or both of the immediate neighbours are free
 */

int Mem_Free(void *ptr){
	/* Your code goes in here */
	if(ptr == NULL){
		return -1;
	}
	
	unsigned int addr = (unsigned int) ptr;
	if (addr % 4 != 0) {
		// ptr is not 4 byte aligned
		return -1;
	}
	if(((void *)first_block) + total_mem_size <= ptr){
		// ptr is not within range of memory
		return -1;
	}
	
	block_tag *header = (block_tag *) (ptr - 4);
	if (header < first_block) {
		// ptr is not with range of memory
		return -1;
	}
	
	if (is_free(header)) {
		return -1;
	}
	
	set_memory_free(header, TRUE);
	block_tag *footer = get_footer(header);
	set_memory_free(footer, TRUE);
	set_block_size(footer, get_block_size(header));
	set_prev_free(footer, FALSE);
	
	// coalesce with the next block if necessary
	block_tag *next = get_next_header(header);
	if (next != NULL) {
		if (is_free(next)) { // coalesce
			unsigned int new_block_size = get_block_size(header) + get_block_size(next);
			set_block_size(header, new_block_size);
			footer = get_footer(next);
			set_block_size(footer, new_block_size);
		} else {
			set_prev_free(next, TRUE);
		}
	}
	
	// coalesce with the prev block if necessary
	if (is_prev_free(header)) {
		block_tag *prev_footer = (block_tag *)(((void *) header) - 4);
		unsigned int prev_size = get_block_size(prev_footer);
		block_tag *prev = (block_tag *)(((void *) header) - prev_size);
		
		unsigned int new_block_size = get_block_size(prev) + get_block_size(header);

		set_block_size(prev, new_block_size);
		set_block_size(footer, new_block_size);
	}
	
	return 0;
}


/*
 *
 * Function used to initialize the memory allocator
 *
 * Not intended to be called more than once by a program
 *
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated
 *
 * Returns 0 on success and -1 on failure
 *
 */
int Mem_Init(int sizeOfRegion){
  int pagesize;
  int padsize;
  int fd;
  int alloc_size;
  void* space_ptr;
  static int allocated_once = 0;
  
  if(0 != allocated_once){
    fprintf(stderr,"Error:mem.c: Mem_Init has allocated space during a previous call\n");
    return -1;
  }
  if(sizeOfRegion <= 0){
    fprintf(stderr,"Error:mem.c: Requested block size is not positive\n");
    return -1;
  }

  // Get the pagesize
  pagesize = getpagesize();

  // Calculate padsize as the padding required to round up sizeOfRegion to a multiple of pagesize
  padsize = sizeOfRegion % pagesize;
  padsize = (pagesize - padsize) % pagesize;

  alloc_size = sizeOfRegion + padsize;

  // Using mmap to allocate memory
  fd = open("/dev/zero", O_RDWR);
  if(-1 == fd){
    fprintf(stderr,"Error:mem.c: Cannot open /dev/zero\n");
    return -1;
  }
  space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (MAP_FAILED == space_ptr){
    fprintf(stderr,"Error:mem.c: mmap cannot allocate space\n");
    allocated_once = 0;
    return -1;
  }
  
  allocated_once = 1;
  
  // Intialising total available memory size
  total_mem_size = alloc_size;

  // To begin with there is only one big free block
  first_block = (block_tag*) space_ptr;
  
  // Setting up the header
  first_block->size_status = alloc_size;
  // Marking the previous block as busy
  first_block->size_status += 2;

  // Setting up the footer
  block_tag *footer = (block_tag*)((char*)first_block + alloc_size - 4);
  footer->size_status = alloc_size;
  
  return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information for each block 
 * No.      : serial number of the block 
 * Status   : free/busy 
 * Prev     : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header)(including the header/footer)
 */ 
void Mem_Dump() {
  int counter;
  char status[5];
  char p_status[5];
  char *t_begin = NULL;
  char *t_end = NULL;
  int t_size;

  block_tag *current = first_block;
  counter = 1;

  int busy_size = 0;
  int free_size = 0;
  int is_busy = -1;

  fprintf(stdout,"************************************Block list***********************************\n");
  fprintf(stdout,"No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
  fprintf(stdout,"---------------------------------------------------------------------------------\n");
  
  while(current < (block_tag*)((char*)first_block + total_mem_size)){

    t_begin = (char*)current;
    
    t_size = current->size_status;
    
    if(t_size & 1){
      // LSB = 1 => busy block
      strcpy(status,"Busy");
      is_busy = 1;
      t_size = t_size - 1;
    }
    else{
      strcpy(status,"Free");
      is_busy = 0;
    }

    if(t_size & 2){
      strcpy(p_status,"Busy");
      t_size = t_size - 2;
    }
    else strcpy(p_status,"Free");

    if (is_busy) busy_size += t_size;
    else free_size += t_size;

    t_end = t_begin + t_size - 1;
    
    fprintf(stdout,"%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n",counter,status,p_status,
                    (unsigned long int)t_begin,(unsigned long int)t_end,t_size);
    
    current = (block_tag*)((char*)current + t_size);
    counter = counter + 1;
  }
  fprintf(stdout,"---------------------------------------------------------------------------------\n");
  fprintf(stdout,"*********************************************************************************\n");

  fprintf(stdout,"Total busy size = %d\n",busy_size);
  fprintf(stdout,"Total free size = %d\n",free_size);
  fprintf(stdout,"Total size = %d\n",busy_size+free_size);
  fprintf(stdout,"*********************************************************************************\n");
  fflush(stdout);
  return;
}
	


