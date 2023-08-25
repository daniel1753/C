
/*********************************
 * Reviewer: Avigail Millatiner			
 * Author: Daniel Binyamin			
 * File: vsa.c					
 * ******************************/

#include <assert.h>/*assert*/
#include "vsa.h"

#define MAGIC (void *)0XABADCAFE 

static void defrag(void* vsa);
static size_t IsFree(size_t curr);

typedef struct block_size
{
	size_t size;
	
	#ifndef NDEBUG
	void *magic_word;
	#endif

}bs_t;


static void* Align(size_t what, size_t to)
{
    return (void*)((what - 1 + to) & -to);
}


vsa_t *VSAInit(void *pool, size_t poolsize)
{
	/*alignment the pool*/
	size_t * new_pool = Align((size_t) pool, sizeof(size_t));
	size_t alined_bs_size =(size_t) Align(sizeof(bs_t), sizeof(size_t));
	bs_t curr_bs = {0};
	
	assert(pool);
	
	
	/*reduce from poolsize the difference from pool ptr after alignment 
	  and the leftover bits in the end*/
	poolsize = (poolsize - ((size_t)new_pool - (size_t)pool)) &
						 ~(sizeof(size_t) -1) ;/*cut the 
						 			leftover*/
	
	/*check if poolsize large enough for 2 meta-data (the curr one and the 
	  "sentinel") and at least one chank of 8*/
	if(poolsize < (size_t)(2 * alined_bs_size + (size_t)8 ))
	{
		return NULL;
	}
	
	/*-1 to mark the sentinel (invalid struct that marks the end of the pool)*/
	curr_bs.size = (int)-1;
	/*stroe it at the end of the pool*/
	*(bs_t*)((char*)new_pool + poolsize - alined_bs_size) = curr_bs;

			     /*one for curr matadata one for sentinel*/
	curr_bs.size = poolsize - (2 * alined_bs_size);
		
	
	#ifndef NDEBUG
    	curr_bs.magic_word = MAGIC;
        #endif
        
	*((bs_t*)new_pool) = curr_bs;
	
	return (vsa_t *)new_pool;
}



void VSAFree(void *block)
{
	assert(((bs_t*)((char*)block - sizeof(bs_t))) -> magic_word == MAGIC);
	
	assert(block);
						/*turn the last bit off*/
	((bs_t*)((char*)block - sizeof(bs_t))) -> size &= ~((size_t)1);
}




void *VSAAlloc(vsa_t *vsa, size_t variable_size)
{
	size_t i = 0;
	size_t curr = 0;
	void * ptr_to_return = NULL;
	size_t alined_bs_size =(size_t) Align(sizeof(bs_t), sizeof(size_t));
	variable_size = (size_t)Align(variable_size, sizeof(size_t));
	
	assert(vsa);      
	
	defrag(vsa);
	
	while((int)curr != -1 )
	{
	
		curr = ((bs_t*)((char*)vsa + i)) -> size;

		/*if curr block is free and in exactly fit size*/
		if(IsFree(curr) && (curr == variable_size))

		{
			ptr_to_return = (char*)vsa + i + alined_bs_size;
			
			/*turn the last bit on*/
			((bs_t*)((char*)vsa + i) )-> size ^= (size_t)1;
			return ptr_to_return;
		
		}
		/*if curr block free and large enough to fit variable_size + 
									matadata*/
		if(IsFree(curr) && curr > variable_size +sizeof(bs_t))
		{
			bs_t curr_bs = {0};
			bs_t next_bs = {0};
			
			/*make the curr occuped*/
			curr_bs.size = variable_size ^1;
			
			#ifndef NDEBUG
    		  	curr_bs.magic_word = MAGIC;
               		#endif
               		
               		/*store it at in place*/
			*(bs_t*)((char*)vsa + i) = curr_bs;
			/*save ptr to return*/	
			ptr_to_return = (char*)vsa + i + alined_bs_size;
			 
			/*Prepare the next metadata*/
			next_bs.size = curr - variable_size- alined_bs_size;
			
			#ifndef NDEBUG
    		  	next_bs.magic_word = MAGIC;
               		#endif
			*(bs_t*)((char*)vsa +
					i +
					alined_bs_size +
					variable_size) = next_bs;
			return ptr_to_return;
		}
	
		/*otherwise (not free) Advance i */
		i += ((curr & ~((size_t)1)) + alined_bs_size);
	}
	
	return NULL;


}


size_t VSALargestChunckAvailable(vsa_t *vsa)
{
	size_t alined_bs_size =(size_t) Align(sizeof(bs_t), sizeof(size_t));
	size_t largest = 0;
	size_t curr = 0;	
	size_t i = 0;
	
	assert(vsa);
	
	defrag(vsa);
	
	
	while((int)curr != -1)
	{
		/*Get the curr block size*/
		curr = ((bs_t*)((char*)vsa + i))-> size;
		
		/*if the curr free and larger then the largest*/
		if(IsFree(curr) &&  largest < curr)
		{
			largest = curr;
		}
		
		i += (curr & (size_t)~1) + alined_bs_size;
	}
	return largest;
}


static void defrag(void * vsa)
{
	size_t i =0 , curr = ((bs_t*)vsa)-> size , next = 0;
	size_t alined_bs_size =(size_t) Align(sizeof(bs_t), sizeof(size_t));

	
	while((int)next != -1)
	{
		/*get the next block size*/
		next = ((bs_t*)((char*)vsa + 
					i + 
					alined_bs_size +
					(curr & ~(size_t)1)))-> size;

		/*(both of them are free)*/
		if(IsFree(next) && IsFree(curr))
		{
			curr += next +alined_bs_size;
		/*	store the sum of them (include the matadata 
						between them) in place*/
			((bs_t*)((char*)vsa + i))-> size = curr ;
			
		}
		else
		{
			/*otherwise advence i and curr*/
			i += (curr & ~(size_t)1) +alined_bs_size;
			curr = next ;
		}
	}
}

/*

static void defrag(void * vsa)
{
	size_t curr = ((bs_t*)vsa)-> size,  next=0;
	size_t alined_bs_size =(size_t) Align(sizeof(bs_t), sizeof(size_t));
	assert(vsa);

	 next = ((bs_t*)((char*)vsa + alined_bs_size + (curr & ~(size_t)1)))-> size;
	
	while((int)next != -1)
	{
		both of them are free
		if(IsFree(next) && IsFree(curr))
		{

			((bs_t*)((char*)vsa + curr))-> size = curr+ next +alined_bs_size;
			next = ((bs_t*)((char * )vsa +curr )) -> size;
		}
		else
		{
			otherwise advence i and curr
			curr = next ;
			next = ((bs_t*)((char*)vsa+next +alined_bs_size) )->size;
		}
	}
}
*/
static size_t IsFree(size_t curr) 
{
	return (!(curr & (size_t)1));
}





















