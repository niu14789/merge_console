
#ifndef __MERGE_MSG_H__
#define __MERGE_MSG_H__


typedef struct
{
	unsigned int index;
	unsigned version;
	char tail[16];
	char  path[256];
} find_files_def;

typedef struct{
   char logo[16];//%<feimarobotics>
   unsigned int check;//check crc
   unsigned int encry;//0:none,1:AES,2:3DES,3:HASH
   unsigned short version;//
   unsigned short rev;//nrs_check_sum
   unsigned int numfw;
   unsigned int fw_start_addr;
   unsigned int fw_size;
   unsigned int offset;
}info;

typedef struct{
	unsigned int index;
    char logo[16];
    unsigned int offet;
	unsigned int size;
	unsigned int sector_num;
	unsigned short version;
	unsigned short config;
	unsigned int check;
}modules_info_def;

typedef struct{
	info nrs_info;//44 bype
    modules_info_def modules_info[15];//40 * 15 = 600;
    unsigned char rev[378];
	unsigned short check_nrs; 
}head_info;

#endif