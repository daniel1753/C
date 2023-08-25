/*********************************
 * Reviewer:  				
 * Author: Daniel Binyamin			
 * File: ext2.c					
 * ******************************/

/* #ifndef __EXT2_H__
#define __EXT2_H__

#include <stdio.h>
#include <ext2fs/ext2_fs.h>


#define BLOCK_OFFSET(block) (block_size * (block))
#define XOPEN_SOURCE 501
#define _POSIX_C_SOURCE 200809L
#define ROOT_INODE_NO 2

typedef struct ext2_super_block sb_t;
typedef struct ext2_inode inode_t;
typedef struct ext2_group_desc g_desc_t;
typedef struct ext2_dir_entry_2 entry_t;


void read_inode(int inode_no, size_t size);
void read_data(char block[]);
int init();
int GetInodeNo(const char *file_name);
sb_t sb;


#endif 
 */
