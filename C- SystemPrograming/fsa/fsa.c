/*********************************
 * Reviewer: 				
 * Author: Daniel Binyamin			
 * File: fsa.c					
 * ******************************/
#include <assert.h>
#include <stdio.h>/*printf*/
#include "fsa.h"

/*static void* Align(size_t what, size_t to);
*/
struct fsa
{
	size_t start;

};

static void* Align(size_t what, size_t to)
{
    return (void*)((what - 1 + to) & -to);
}

fsa_t *FSAInit(void *pool, size_t pool_size, size_t block_size)
{
	size_t i =0;
	size_t aligned_block_size = 0;
	fsa_t * fsa = Align((size_t)pool , sizeof(size_t));

	assert(pool);
	
	fsa->start = (size_t) Align(sizeof(fsa_t), sizeof(size_t));
	aligned_block_size = (size_t)Align(block_size, sizeof(size_t));
	pool_size = pool_size - ((size_t)fsa - (size_t)pool ) - fsa->start;
	
/*add a retun if poolsize is not lerge engh
*/

	for(i=fsa->start; i < pool_size-aligned_block_size; i += aligned_block_size)
	{
	
		*((size_t*)((char*)fsa + i )) = i + aligned_block_size;
	}
	
	*((size_t*)((char*)fsa + i))  = (int)-1;
	
	return fsa;
}



void *FSAAlloc(fsa_t *fsa)
{
	size_t *to_return = NULL;
	assert(fsa);
	
	if((int)fsa->start == -1)
	{
		return NULL;
	}
	
	to_return = (size_t*)((char*)fsa + fsa->start);
	fsa->start = *to_return;
	
	return (void*)to_return;
	
}

void FSAFree(fsa_t *fsa, void *block_address)
{
	size_t to_free = ((size_t)block_address) - ((size_t)fsa);
	
	assert(fsa);
	assert(block_address);
	
	*(size_t *)block_address = fsa -> start;
	fsa->start = to_free;
}

size_t FSACountFree(fsa_t *fsa)
{
	size_t curr =  0;
	size_t counter = 0;
	int check = 0;
	
	assert(fsa);

	if ((int)(fsa->start) == -1)
	{
		return (size_t)0;
	}
	 curr = fsa->start;

	
	while(check != -1 )
	{
		check = *((int*)((char*)fsa + curr));
		curr = *((size_t*)((char*)fsa + curr));
		++counter;
	}
	return counter ;

}

size_t FSASuggestSize(size_t block_size, size_t num_of_blocks)
{
	size_t aligned_block_size = (size_t) Align(block_size, sizeof(size_t));
	size_t aligned_fsa_size = (size_t) Align(sizeof(fsa_t), sizeof(size_t));
	return sizeof(size_t) -1 +aligned_fsa_size+(num_of_blocks* aligned_block_size);
}




/*for me dont check
void Printer(fsa_t * fsa)
{
	size_t curr =  0;
	size_t next =  fsa->start;
	
	while((int)next != -1 )
	{
		curr = *((size_t*)fsa + curr);
		next = *((size_t*)fsa + next);
		
		printf("curr: %ld       next is: %ld  \n",curr , next);
		
		
	}
}

*/






