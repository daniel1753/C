/*********************************
 * Reviewer: 				
 * Author: Daniel Binyamin			
 * File: fsa.c					
 * ******************************/


#ifndef __FSA_H__
#define __FSA_H__

#include <stddef.h>/*size_t*/

typedef struct fsa fsa_t;

void Printer(fsa_t * fsa);

fsa_t *FSAInit(void *pool, size_t pool_size, size_t block_size);
/*	Description: Creates an admin Fixed-Size Allocator.
	Parameters: Pointer to the memory pool,pool_size, block size.
	Return Value: Pointer to admin Fixed-Size Allocator if success.
					NULL if failed or if pool is NULL.
	Undefined Behavior: Invalid block size.
	Complexity: O(n)
*/

void *FSAAlloc(fsa_t *fsa);
/*	Description: Allocate memory from memory pool.
	Parameters: Pointer to admin Fixed-Size Allocator.
	Return Value: Pointer (aligned to size_t) to allocated block.
				  NULL if failed.
	Undefined Behavior:
	Complexity: O(1)
*/

void FSAFree(fsa_t *fsa, void *block_address);
/*	Description: Free the block.
	Parameters: Pointer to admin Fixed-Size Allocator.
				Pointer to block address.
	Return Value: None
	Undefined Behavior: Block address isn't in the memory pool.
	Complexity: O(1)
*/

size_t FSACountFree(fsa_t *fsa);
/*	Description: Counts vacant blocks currently held in memory pool.
	Parameters: Pointer to admin Fixed-Size Allocator.
	Return Value: Number of vacant blocks.
	Undefined Behavior:
	Complexity: O(n)
*/

size_t FSASuggestSize(size_t block_size, size_t num_of_blocks);
/*	Description: The user will tell us the size and amount of blocks,
				 The function will return a suggested size of the required pool.
	Parameters: Block size in bytes, Number of blocks.
	Return Value: Size of the suggested memory pool (in bytes).
	Undefined Behavior: 
	Complexity: O(1)
*/

#endif /* __FSA_H__ */
