/*********************************
 * Reviewer: Daniel Natan
 * Author: Irit Ofir
 * File: vsa_test.c
 * ******************************/

#include <stdlib.h> /*malloc, free*/
#include <stdio.h> /*printf*/
#include <stddef.h> /*size_t*/
#include <string.h> /*strncpy*/
#include "vsa.h" 

typedef struct size12
{
    char str[12];
} size12_t;

typedef struct size5
{
    char str[5];
} size5_t;

typedef struct size78
{
    char str[78];
} size78_t;

size12_t InitSize12(char *str)
{
    size12_t struct12 = {0};
    strncpy(struct12.str, str, 12);

    return struct12;
}

size5_t InitSize5(char *str)
{
    size5_t struct5 = {0};
    strncpy(struct5.str, str, 5);

    return struct5;
}

size78_t InitSize78(char *str)
{
    size78_t struct78 = {0};
    strncpy(struct78.str, str, 78);

    return struct78;
}

void TestInit()
{
    void *pool = malloc(100);
    vsa_t *vsa = VSAInit(pool, 5);

    if (vsa)
    {
        printf("TestInit fail (return null) on line %d\n", __LINE__);
    }

    vsa = VSAInit(pool, 100);

    if (!vsa)
    {
        printf("TestInit fail (return pointer) on line %d\n", __LINE__);
    }

    free(pool);
}

void TestAlloc()
{
    void *pool = malloc(140);
    vsa_t *vsa = VSAInit(pool, 140);

    size78_t *ptr1 = VSAAlloc(vsa, 78);
    size78_t *ptr2 = VSAAlloc(vsa, 78);

    if (!ptr1)
    {
        printf("TestAlloc failed (return pointer) on line %d\n", __LINE__);
    }

    if (ptr2)
    {
        printf("TestAlloc failed (return null) on line %d\n", __LINE__);
    }

    free(pool);
}

void TestAllocFree()
{
    char *pool = malloc(200);
    vsa_t *vsa = VSAInit(pool + 1, 199);

    size5_t *ptr1_5 = VSAAlloc(vsa, 5);
    size12_t *ptr2_12 = VSAAlloc(vsa, 12);
    size78_t *ptr3_78 = VSAAlloc(vsa, 78);
    size5_t *ptr4_5 = VSAAlloc(vsa, 5);
    size78_t *ptr5_78 = VSAAlloc(vsa, 78);
    size12_t *ptr6_12 = NULL;
    size5_t *ptr7_5 = NULL;
    size78_t *ptr8_78 = NULL;

    *ptr1_5 = InitSize5("irit");
    *ptr2_12 = InitSize12("tamir ofir");
    *ptr3_78 = InitSize78("troy and abbed in the morning");
    *ptr4_5 = InitSize5("max");

    if (ptr5_78)
    {
        printf("TestAllocFree failed (return null on full) on line %d\n", 
                __LINE__);
    }

    VSAFree(ptr3_78);
    ptr3_78 = NULL;

    ptr6_12 = VSAAlloc(vsa, 12);

    if (!ptr6_12)
    {
        printf("TestAllocFree failed (return pointer after free) on line %d\n",
            __LINE__);
    }

    *ptr6_12 = InitSize12("max & lewis");

    VSAFree(ptr2_12);
    ptr2_12 = NULL;

    ptr7_5 = VSAAlloc(vsa, 5);

    *ptr7_5 = InitSize5("eitan");

    if (!ptr7_5)
    {
        printf("TestAllocFree failed (return pointer after free) on line %d\n",
            __LINE__);
    }

    ptr8_78 = VSAAlloc(vsa, 78);

    if (ptr8_78)
    {
        printf("TestAllocFree failed (return null after free) on line %d\n", 
                __LINE__);
    }

    free(pool);
}

void TestLargestChunk()
{
    char *pool = malloc(200);
    vsa_t *vsa = VSAInit(pool + 5, 195);
    size_t chunk1 = 0, chunk2 = 0;

    size78_t *ptr1_78 = NULL, *ptr2_78 = NULL;
    size12_t *ptr1_12 = NULL;
    size5_t *ptr1_5 = NULL;

    ptr1_78 = VSAAlloc(vsa, 78);
    ptr2_78 = VSAAlloc(vsa, 78);

    *ptr1_78 = InitSize78("oh behave");

    if (ptr2_78)
    {
        printf("Pool size too large for test on line %d\n", __LINE__);
    }

    chunk1 = VSALargestChunckAvailable(vsa);

    VSAFree(ptr1_78);
    ptr1_78 = NULL;

    chunk2 = VSALargestChunckAvailable(vsa);

    if (chunk1 > chunk2)
    {
        printf("TestLargestChunk failed on line %d\n", __LINE__);
    }

    ptr1_78 = VSAAlloc(vsa, 78);
    ptr1_12 = VSAAlloc(vsa, 12);
    ptr1_5 = VSAAlloc(vsa, 5);

    *ptr1_78 = InitSize78("bounce! pogo pogo pogo pogo pogo pogo pogo");
    *ptr1_12 = InitSize12("toxicity");
    *ptr1_5 = InitSize5("what");

    chunk1 = VSALargestChunckAvailable(vsa);

    if(chunk1 > chunk2)
    {
        printf("TestLargestChunk failed on line %d\n", __LINE__);
    }

    VSAFree(ptr1_78);
    ptr1_78 = NULL;

    if (chunk1 > VSALargestChunckAvailable(vsa))
    {
        printf("TestLargestChunk failed on line %d\n", __LINE__);
    }

    free(pool);
}

void TestDefragment()
{
    char *pool = malloc(250);
    vsa_t *vsa = VSAInit(pool, 250);

    size78_t *ptr1_78 = NULL;
    size12_t *ptr1_12 = NULL, *ptr2_12 = NULL, *ptr3_12 = NULL;
    size5_t *ptr1_5 = NULL, *ptr2_5 = NULL, *ptr3_5 = NULL;

    ptr1_12 = VSAAlloc(vsa, 12);
    *ptr1_12 = InitSize12("it's just");

    ptr1_5 = VSAAlloc(vsa, 5);
    *ptr1_5 = InitSize5("one");

    ptr2_12 = VSAAlloc(vsa, 12);
    *ptr2_12 = InitSize12("of those");

    ptr2_5 = VSAAlloc(vsa, 5);
    *ptr2_5 = InitSize5("days");

    ptr3_12 = VSAAlloc(vsa, 12);
    *ptr3_12 = InitSize12("feeling");

    ptr3_5 = VSAAlloc(vsa, 5);
    *ptr3_5 = InitSize5("like");

    
    ptr1_78 = VSAAlloc(vsa, 78);

    if (ptr1_78)
    {
        printf("Pool size too large for test on line %d\n", __LINE__);
    }

    
    VSAFree(ptr1_5);
    ptr1_5 = NULL;
    
    VSAFree(ptr2_12);
    ptr2_12 = NULL;
    
    VSAFree(ptr2_5);
    ptr2_5 = NULL;
    
    VSAFree(ptr3_12);
    ptr3_12 = NULL;

    /*printf("%lu\n", VSALargestChunckAvailable(vsa));*/

    VSAFree(ptr3_5);
    ptr3_5 = NULL;
    
    ptr1_78 = VSAAlloc(vsa, 78);
    *ptr1_78 = InitSize78("a freight train, first one to complain \
                           leaves with a bloodstain");

    if (!ptr1_78)
    {
        printf("TestDefragment failed on line %d\n", __LINE__);
    }

    free(pool);
}


void Test40()
{
    char *pool = malloc(40);
    vsa_t *vsa = VSAInit(pool, 40);

    void *ptr1 = VSAAlloc(vsa, 4);

    void *ptr2 = VSAAlloc(vsa, 4);

    
    
    free(pool);
}

int main()
{
    TestInit();
    TestAlloc();
    TestAllocFree();
    TestLargestChunk();
    TestDefragment();
    Test40();
    return 0;
}
