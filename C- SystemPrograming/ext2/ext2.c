/*********************************
 * Reviewer:  		
 * Author: Daniel Binyamin			
 * File: ext2.c					
 * ******************************/


#include <stdlib.h>
#include <string.h> /* memcpy */
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>/* ISIDR */

#include "ext2.h"

/* size_t block_size = sizeof(sb_t);

g_desc_t g_des;
inode_t root_inode;
int fd = 0;

int init()
{
	if( 0 > (fd = open("/dev/ram0", O_RDONLY)))
	{
		printf("open failed\n");
	}

	lseek(fd,block_size,SEEK_SET);
	if( 0 >= (read(fd, &sb, sizeof(sb_t ))))
	{
		printf("read super block failed\n");
	}

	block_size = block_size << sb.s_log_block_size;
	lseek(fd,block_size,SEEK_SET);
	if( 0 >= (read(fd, &g_des, sizeof(g_desc_t))))
	{
		printf("read g_des failed\n");
	}

	read_inode(ROOT_INODE_NO, sizeof(inode_t));

	return fd;
}


int GetInodeNo(const char *file_name )
{
	entry_t *entry;
	char block[block_size];
	char buffer[EXT2_NAME_LEN+1];
	int i =0, res=1;

	if (!S_ISDIR(root_inode.i_mode)) 
	{
		printf("no DIR\n");

	}
	read_data(block);

	entry = (entry_t *)block;
	
	for(i =0 ; i< root_inode.i_size && res != 0; 
		i += entry->rec_len , entry = (entry_t*)(((char*)entry )+ entry->rec_len))
	{
        memcpy(buffer, entry->name, entry->name_len);
        buffer[entry->name_len] = 0;             
        printf("%u %s\n", entry->inode, buffer);
		
		res = strcmp(buffer,file_name);
	}

	return entry->inode-2;
}

void read_inode(int inode_no, size_t size)
{
	lseek(fd,BLOCK_OFFSET(g_des.bg_inode_table)+ (inode_no) * size, SEEK_SET);
	read(fd, &root_inode, sizeof(inode_t));
}

void read_data(char block[])
{

	lseek(fd, BLOCK_OFFSET(root_inode.i_block[0]), SEEK_SET);
	if( 0 >= (read(fd, block, block_size)))
	{
		printf("read block failed\n");
	}
} */