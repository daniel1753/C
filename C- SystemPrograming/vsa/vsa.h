/*********************************
 * Reviewer: 				
 * Author: Daniel Binyamin			
 * File: vsa.c					
 * ******************************/

#ifndef __VSA_H__
#define __VSA_H__

#include <stddef.h>	/*size_t*/

typedef struct vsa vsa_t;


vsa_t *VSAInit(void *pool, size_t poolsize);
/*	Description: Creates an admin Variable-Size Allocator.
	Parameters: Pointer to the memory pool and pool size.
	Return Value: Pointer to admin Fixed-Size Allocator if success.
				  NULL if failed or if pool is NULL.
	Undefined Behavior: 
	Complexity: O(1)
*/

void *VSAAlloc(vsa_t *vsa, size_t variable_size);
/*	Description: Allocate memory from memory pool.
				 Allocation according first fit method
	Parameters: Pointer to admin Variable-Size Allocator and desired variable size
	Return Value: Pointer (aligned to size_t) to allocated block.
				  NULL if failed.
	Undefined Behavior: Invalid variable size
	Complexity: O(n)
*/

void VSAFree(void *block);
/*	Description: Free the block.
	Parameters: Pointer to block address.
	Return Value: None
	Undefined Behavior: Block address isn't in the memory pool.
	Complexity: O(1)
*/

size_t VSALargestChunckAvailable(vsa_t *vsa);
/*	Description: Finds the size of the largest memory chunk available in the pool.
	Parameters: Pointer to admin Variable-Size Allocator.
	Return Value: Size of the Largest chunk available in the pool.
	Undefined Behavior:
	Complexity: O(n)
*/


#endif /* __VSA_H__ */
