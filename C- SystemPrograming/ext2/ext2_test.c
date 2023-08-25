/*********************************
 * Reviewer:
 * Author: Daniel Binyamin
 * File: ext2_test.c
 * ******************************/
/*  BIFORE U COMPIL ************************************ 
sudo modprobe brd              
sudo mkfs.ext2 /dev/ram0        format the "divice" to ext2
sudo mount /dev/ram0  /mnt      munt it to folder /mnt
sudo cp /usr/include/*.h /mnt/  cpy the usr h files to the divice
ls /mnt                         
sudo umount /mnt                 unmount it
***** dont forget to *******sudo ./ext2.Debug.out**********
 */

#define BLOCK_OFFSET(block) (block_size * (block))
#define XOPEN_SOURCE 501
#define _POSIX_C_SOURCE 200809L
#define ROOT_INODE_NO 2

#include <stdlib.h>
#include <string.h> /* memcpy */
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>/* ISIDR */
#include <ext2fs/ext2_fs.h>
#include "ext2.h"

typedef struct ext2_super_block sb_t;
typedef struct ext2_inode inode_t;
typedef struct ext2_group_desc g_desc_t;
typedef struct ext2_dir_entry_2 entry_t;
unsigned int size;
size_t block_size = sizeof(sb_t);
sb_t sb;
g_desc_t g_des;
inode_t root_inode;
int fd = 0;

static void read_inode(int inode_no, size_t size);
static void read_data(int block_size , char block[]);


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
	unsigned int i =0, res=1;

	if (!S_ISDIR(root_inode.i_mode)) 
	{
		printf("no DIR\n");

	}
	read_data(block_size , block);

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

int main(int argc, char *argv[])
{
	init();
	char block[block_size];

	if(argc < 1)
	{
		printf("no filename\n");
	}

	read_inode(GetInodeNo(argv[1]), sb.s_inode_size);

	read_data(block_size , block);
	printf("name:%s\n",block);

	
	return 0;
}

static void read_inode(int inode_no, size_t size)
{
	lseek(fd,BLOCK_OFFSET(g_des.bg_inode_table)+ (inode_no) * size, SEEK_SET);
	read(fd, &root_inode, sizeof(inode_t));
}

static void read_data(int block_size , char block[])
{

	lseek(fd, BLOCK_OFFSET(root_inode.i_block[0]), SEEK_SET);
	if( 0 >= (read(fd, block, block_size)))
	{
		printf("read block failed\n");
	}
}