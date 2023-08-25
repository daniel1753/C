/*********************************
 * Reviewer:
 * Author: Daniel Binyamin
 * File: shuffleAndSort_test.c
 * ******************************/
#define FACTOR 1
#define _POSIX_C_SOURCE  199310L
#define NUM_OF_THEARDS 1

#include <sys/time.h>/* timeofday */
#include <stdio.h> /* FILE */
#include <sys/mman.h> /* mmap */
#include <string.h> /* strcmp */
#include <pthread.h>/* tread */
#include <unistd.h>/* tread */
#include <stdlib.h> /* malloc */
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

int MergeSort(void *arr_to_sort, size_t num_elements, size_t elem_size,size_t stop, int(*is_before)(const void *elem1, const void *elem2));
static void MergeRecursive( void *arr_to_sort, 
                            void* buffer, 
                            size_t left,  
                            size_t right, 
                            size_t elem_size, 
                            size_t stop,
                            int(*is_before)(const void *elem1, 
                                            const void *elem2));
static void SortBuff(   void *arr_to_sort, 
                        void* buffer , 
                        size_t left,
                        size_t mid,
                        size_t right , 
                        size_t elem_size, 
                        int(*is_before)(const void *elem1, 
                                        const void *elem2));
static int is_before(const void *elem1, const void *elem2);

void CheckSort(char **ptr_arr, size_t size);
static char *GetDB(int *size);
static int GetWordsNum(size_t size, char *start);
static char **shuffle(size_t *num_of_words);

void *sortFunc(void *wordsArr);

int compar_suff(const void *elem1, const void *elem2);
int str_compar(const void *elem1, const void *elem2);

typedef struct words_arr
{
    char **start_point; 
    size_t num_of_words;
} words_arr_t;

static double time_diff(struct timeval x , struct timeval y)
{
    return ((x.tv_sec*1000000 + x.tv_usec) - (y.tv_sec*1000000 + y.tv_usec)) / 1000000.0;
}

int main()
{
    int i =0 , remainder = 0;
    char **ptr_arr=NULL;
    size_t num_of_words =0 , size =0;
    words_arr_t *arr[NUM_OF_THEARDS] = {0};
    struct timeval t1, t2;
    
    pthread_t thread[NUM_OF_THEARDS] = {0};

    ptr_arr = shuffle(&num_of_words);
    size = num_of_words;
    remainder = num_of_words % NUM_OF_THEARDS;

    num_of_words /= NUM_OF_THEARDS;

    for(i =0 ; i< NUM_OF_THEARDS ; ++i)
    {
        arr[i] = malloc(sizeof(words_arr_t));
    }
    gettimeofday(&t1, NULL);
    for(i=0 ; i< NUM_OF_THEARDS ; ++i)
	{
        arr[i]->num_of_words = num_of_words;
        if(remainder > 0)
        {
            ++arr[i]->num_of_words;
            --remainder;
        }
        arr[i]->start_point = ptr_arr +(i*arr[i]->num_of_words);
		pthread_create(&thread[i] , NULL, sortFunc, arr[i]);
	}

    gettimeofday(&t2, NULL);
    printf("time:%f\n", time_diff(t2,t1));
    
    for(i=0 ; i < NUM_OF_THEARDS ; ++i)
	{

		pthread_join(thread[i], NULL);
	}

    MergeSort(ptr_arr,num_of_words,sizeof(char*), num_of_words,is_before);

    CheckSort(ptr_arr, size);

    for(i =0 ; i< NUM_OF_THEARDS ; ++i)
    {
        free(arr[i] );
    }
    
    return 0;
}

void CheckSort(char **ptr_arr, size_t size)
{
    size_t i=0, worg=0;
    
    for (i = 0; (size_t)i < size  ; ++i)
    {

        if(0 < strcmp(ptr_arr[i],ptr_arr[i+1]))
        {
            worg++;
        }
           /*  printf("i = %s\ti+1 = %s\n",ptr_arr[i],ptr_arr[i+1]); */
    }
    if(worg == 0)
    {
        printf("sort secessed\n");
    }


}

int MergeSort(  void *arr_to_sort, 
                size_t num_elements,   
                size_t elem_size,
                size_t stop,
                int(*is_before)(const void *elem1, 
                                const void *elem2))
{
    void *buff= NULL;
    size_t arr_size =num_elements * elem_size;

    assert(arr_to_sort);
    assert(is_before);


    buff = (void*)malloc(arr_size);
    if(!buff)
    {
        return 1;
    }

    MergeRecursive(arr_to_sort ,
                    buff, 
                    0 ,
                    num_elements - 1, 
                    elem_size,
                    stop, 
                    is_before);

    free(buff);
    buff = NULL;

    return 0;

}


static void MergeRecursive( void *arr_to_sort, 
                            void* buffer, 
                            size_t left,  
                            size_t right, 
                            size_t elem_size,
                            size_t stop, 
                            int(*is_before)(const void *elem1, 
                                            const void *elem2))
{
   /* todo change the condition to when each part is in num_of_words/num_of_threads! */
    size_t size = right -left,
    mid=0;

    if(left >= right || stop == size)
    {
        return;
    }
        mid = left + (right -left) /2;

        MergeRecursive( arr_to_sort ,
                        buffer, 
                        left , 
                        mid ,
                        elem_size ,
                        stop,
                        is_before);

        MergeRecursive( arr_to_sort ,
                        buffer, 
                        mid +1 , 
                        right ,
                        elem_size,
                        stop,
                        is_before);

        SortBuff(   arr_to_sort ,
                    buffer, 
                    left , 
                    mid , 
                    right, 
                    elem_size, 
                    is_before);
    
    
}

static void SortBuff(   void *arr_to_sort, 
                        void* buffer , 
                        size_t left,
                        size_t mid,
                        size_t right , 
                        size_t elem_size, 
                        int(*is_before)(const void *elem1, 
                                        const void *elem2))
{
    size_t left_len = mid - left +1;
    size_t right_len = right - mid;
    int i =0 , j =0 , k=0;


    memcpy(( (char*)buffer)+left *elem_size, 
                ((char*)arr_to_sort)+left*elem_size, 
                left_len*elem_size);
      
    memcpy(  (void *)((char*)buffer + (mid +1) * elem_size) , 
            (void *)((char*)arr_to_sort + (mid+1) * elem_size) ,
            right_len * elem_size);

    for(i = 0, j =0 , k= left; ((size_t)i < left_len && 
                                (size_t)j < right_len); ++k)
    {

    if(is_before((  (char*)buffer)+(left +i) *elem_size,
                    (char*)buffer+(mid +1 +j) *elem_size))
    {

        memcpy( ((char*)arr_to_sort) + k * elem_size, 
                ((char*)buffer) + (left +i) * elem_size, 
                elem_size );  
        ++i;
    }
    else
    {
        memcpy( ((char*)arr_to_sort) + k * elem_size , 
                ((char*)buffer)+(mid +1 + j) * elem_size, 
                elem_size );  
        ++j;
    }
    }


    memcpy( ((char*)arr_to_sort) + k * elem_size, 
                ((char*)buffer)+(left +i)*elem_size, 
                (left_len-i)*elem_size);  

    memcpy( ((char*)arr_to_sort) + k * elem_size , 
                ((char*)buffer)+(mid +1+ j) *elem_size,
                (right_len -j)* elem_size);  
}

void *sortFunc(void *wordsArr)
{
    qsort(((words_arr_t*)wordsArr)->start_point, ((words_arr_t*)wordsArr)->num_of_words, sizeof(char *), str_compar); 

    return NULL;
}

char *GetDB(int *size)
{
    int fd = 0;
    char *buff = NULL;
    struct stat statbuff = {0};

    fd = open("/usr/share/dict/words", O_RDONLY);
    fstat(fd, &statbuff);

    if (fd < 0)
    {
        printf(" could not open\n");
        return NULL;
    }
    buff = (char *)mmap(0,
                        statbuff.st_size,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE,
                        fd,
                        0);

    if (!buff)
    {
        printf("no shuffle todat\n");
        return NULL;
    }

    *size = statbuff.st_size;
    return buff;
}

int GetWordsNum(size_t size, char *start)
{
    size_t num_of_words = 0;
    char *end = start;

    while (size)
    {
        ++num_of_words;
        start = memchr(end, '\n', size);
        *start = '\0';
        start++;
        size -= start - end;
        end = start;
    }
    return num_of_words;
}

char **shuffle(size_t *num_of_words)
{
    char *tmp_buff = NULL, *helper = NULL;
    char *buff = NULL;
    char **ptr_arr = NULL;
    int size = 0,  i = 0, j = 0;

    buff = GetDB(&size);
    helper = buff;
    tmp_buff = helper;

    *num_of_words = (GetWordsNum(size, buff) * FACTOR);

    ptr_arr = (char **)malloc(sizeof(char *) * *num_of_words );
    if (!ptr_arr)
    {
        printf("ptr_arr alloctaion failed\n");
        return NULL;
    }

    tmp_buff = buff;
    for (i = 0; (size_t)i < *num_of_words ; i += FACTOR)
    {
        for (j = 0; j < FACTOR; ++j)
        {
            ptr_arr[i + j] = tmp_buff;
        }
        tmp_buff = memchr(tmp_buff, '\0', size);
        ++tmp_buff;
        size -= strlen(tmp_buff);
    }

    for(i=0; i<3 ; ++i)
    {
        qsort(ptr_arr, *num_of_words, sizeof(char *), compar_suff);
    }

    return ptr_arr;    
}


int is_before(const void *elem1, const void *elem2)
{
    if(0 > strcmp(*(char **)elem1, *(char **)elem2))
    {
        return 1;
    }
    return 0;
}

int str_compar(const void *elem1, const void *elem2)
{
    return strcmp(*(char **)elem1, *(char **)elem2);
}

int compar_suff(const void *elem1, const void *elem2)
{
    int num = (rand() % 15);
    (void)elem1;
    (void)elem2;
    if (num <= 5)
    {
        return 1;
    }
    if (num <= 10)
    {
        return 0;
    }
    return -1;
}