
#include "stdafx.h"
#include<stdio.h>  
#include<string.h>  
#include "afx.h"
#include <stdio.h>
/*------------------*/

#define ENABLE  (0)
#define DIASBLE (1)

int des_encrypt(char * ming, char *key, char * out, unsigned char len);
int des_decrypt(char * mi, char *key, char * out, unsigned char len);
int dencrypt_file(const char *path, char * key, unsigned int mode);
int Tchar_to_char(_TCHAR * tchar, char * buffer);
int FindBmpFile(CString strFoldername);
/* about find */
static char name_buffer[20][200];
static unsigned int command = 0;
static unsigned int cmd = 0;
static char * path, *file;
static char key[9] = "<18A5DF>";
CString current_path;
static unsigned int file_find_cnt = 0 , do_cnt = 0;
char *dds, *path_d;
/* */
int _tmain(int argc, _TCHAR* argv[])
{
	/* get data */
	for (int i = 1; i < argc; i++)
	{
		Tchar_to_char(argv[i], name_buffer[i - 1]);
	}
	/*--------------------*/
	if (strcmp(name_buffer[0], "e") == 0)//encrypt a file or a path
	{
		command = ENABLE;
	}
	else if (strcmp(name_buffer[0], "d") == 0)//decrypt a file or a path
	{
		command = DIASBLE;
	}
	else
	{
		printf("Unknow command : %s\r\n", name_buffer[0]);
		return 0;
	}
	/* get file or a path */
	if (strcmp(name_buffer[1], "-path") == 0)
	{
		cmd = 0;
		path = name_buffer[2];
	}
	else if (strcmp(name_buffer[1], "-f") == 0)
	{
		cmd = 1;
		file = name_buffer[2];
	}
	else
	{
		printf("Unknow command : %s\r\n", name_buffer[1]);
		return 0;
	}
	/*--------------------*/
	//do 
	if (cmd == 0)//path
	{
		USES_CONVERSION;
		current_path = A2T(path);
		FindBmpFile(current_path);
		/* ok */
		printf("Find %d %s:%d\r\n", file_find_cnt, command ? "Decrypty" : "Encrypty", do_cnt);
	}
	else if( cmd == 1 )//file
	{
		dencrypt_file(file, key, command);
	}
    /* retun */
	return 0;
}
/* tchar to char */
int Tchar_to_char(_TCHAR * tchar, char * buffer)
{
	int i = 0;
	memset(buffer, 0, sizeof(buffer));

	while (*tchar != '\0')
	{
		buffer[i] = (char)(*tchar);
		tchar++;
		i++;
	}
	return i;
}
/* find a path and do it */
int FindBmpFile(CString strFoldername)
{
	CFileFind tempFind;
	BOOL bFound;
	bFound = tempFind.FindFile(strFoldername + "\\*.*");
	CString strTmp, strpath;
	/*-----------*/
	while (bFound)
	{
		bFound = tempFind.FindNextFile();

		if (tempFind.IsDots())
			continue;

		if (tempFind.IsDirectory())
		{
			strTmp = _T(""); 
			strTmp = tempFind.GetFilePath();
			current_path = strTmp;
			FindBmpFile(strTmp);
		}
		else
		{
			strTmp = tempFind.GetFileName();
			strpath = tempFind.GetFilePath();
			/*file_find_cnt*/
			file_find_cnt++;
			/*----------*/
			USES_CONVERSION;
			/*--------------------------*/
			dds = T2A(strTmp);
			path_d = T2A(strpath);
			/*--------------------------*/
			if (dencrypt_file(path_d, key, command) == 0)
			{
				printf("%s:%s OK\r\n", command?"Decrypty":"Encrypty", path_d);
				do_cnt++;// decrypty or enctypty ++;
			}
		}
	}
	/*------------------------------*/
	tempFind.Close();
	/*---------------*/
	return 0;
}