// FINDX.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "afx.h"
#include <stdio.h>
#include <winsock.h>
#include "merge_msg.h"

static char *fw_path[15];

static unsigned char open_flag_m[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static unsigned char write_buffer[ 1024 * 1024 * 20 ]; // max to 20 MByte
static unsigned int write_addr = 0;

static unsigned char read_buffer[ 2 * 1024 * 1024 ];// 2 MByte

const unsigned int version_export[3] = {0xeabc2547,0,0x3526ec88};

CString dis;
static FILE * file[15];
static FILE * sunFile;

/*---------------------------*/
void Merge_process();
void info_init( head_info * info );
unsigned check_cal(unsigned char * data , unsigned int len);
unsigned short get_version(unsigned char * data,unsigned int len , unsigned int index );
int FM_file_name(char * name_buffer,const char * path,unsigned short fc_version);
/*---------------------------*/
const char logo_index[15][16] = { 
	"gps board",//0
    "bottom board",//1
    "mag1 board",//2
    "mag2 board",//3
	"sonar board",//4
    "gimbal_1 board",//5
	"gimbal_5 board",//6
	"ESC1 board",//7
    "ESC2 board",//8
    "ESC3 board",//9
    "ESC4 board",//10
    "FC board",//11
    "RX1 board",//12
    "video board",//13
    "radio board"};//14


static find_files_def find_files[15];

static char name_buffer[128][200];

//#pragma  comment(  linker,  "/subsystem:\"windows\"  /entry:\"wmainCRTStartup\""  )

unsigned int file_find_cnt = 0,file_add_cnt = 0;

char * dds,*path_g;

CString current_path;

int Tchar_to_char(_TCHAR * tchar,char * buffer);

int FindBmpFile(CString strFoldername)
{
    CFileFind tempFind;
    BOOL bFound; 
    bFound=tempFind.FindFile(strFoldername +   "\\*.*"); 
    CString strTmp,strpath;   //如果找到的是文件夹 存放文件夹路径
	/*-----------*/
    while(bFound)      //遍历所有文件
    {
        bFound=tempFind.FindNextFile(); 
                                                              
        if(tempFind.IsDots()) 
			continue; 

			if(tempFind.IsDirectory())   
            {
                strTmp = _T("");
                strTmp = tempFind.GetFilePath();
				current_path = strTmp;
                FindBmpFile(strTmp);
            }
            else
            {
				strTmp = tempFind.GetFilePath();
                 // 在此处添加对找到文件的处理
				strpath = tempFind.GetFilePath();
				/*file_find_cnt*/
				file_find_cnt ++;
				/*----------*/
				USES_CONVERSION;
				/* change path to char */
				dds = T2A(tempFind.GetFileName());
				/*-----------------------------*/
				path_g = T2A(strpath);
				/* case the file name */
				if( sscanf(dds,"D200_GPS_v%d.%s",&find_files[0].version,find_files[0].tail) == 2 )
				{		
					{ 
						open_flag_m[0] = 1;
						memcpy(find_files[0].path,path_g,strlen(path_g));
						fw_path[0] = find_files[0].path; 
//						printf("GPS Firmware Found ! Verion = %d type = %s \r\n%s\r\n",find_files[0].version,find_files[0].tail,find_files[0].path);
					}
				}else if( sscanf(dds,"D200_BOTTOM_v%d.%s",&find_files[1].version,find_files[1].tail) == 2 )
				{
					{
						memcpy(find_files[1].path,path_g,strlen(path_g));
						fw_path[1] = find_files[1].path;  
						open_flag_m[1] = 1;
//						printf("Bottom Firmware Found ! Verion = %d type = %s \r\n",find_files[1].version,find_files[1].tail);
					}
				}else if( sscanf(dds,"D200_MAG_v%d.%s",&find_files[2].version,find_files[2].tail) == 2 )
				{
					memcpy(find_files[2].path,path_g,strlen(path_g));
					fw_path[2] = find_files[2].path;  
					open_flag_m[2] = 1;
					fw_path[3] = find_files[2].path;  
					open_flag_m[3] = 1;
//					printf("MAG Firmware Found ! Verion = %d type = %s \r\n",find_files[2].version,find_files[2].tail);
				}else if( sscanf(dds,"D200_SONAR_v%d.%s",&find_files[3].version,find_files[3].tail) == 2 )
				{
					memcpy(find_files[3].path,path_g,strlen(path_g));
					fw_path[4] = find_files[3].path;  
					open_flag_m[4] = 1;
//					printf("SONAR Firmware Found ! Verion = %d type = %s \r\n",find_files[3].version,find_files[3].tail);
				}else if( sscanf(dds,"D200_GIMBAL1_v%d.%s",&find_files[4].version,find_files[4].tail) == 2 )
				{
					memcpy(find_files[4].path,path_g,strlen(path_g));
					fw_path[5] = find_files[4].path;  
					open_flag_m[5] = 1;
//					printf("GIMBAL1 Firmware Found ! Verion = %d type = %s \r\n",find_files[4].version,find_files[4].tail);
				}else if( sscanf(dds,"D200_GIMBAL5_v%d.%s",&find_files[5].version,find_files[5].tail) == 2 )
				{
					memcpy(find_files[5].path,path_g,strlen(path_g));
					fw_path[6] = find_files[5].path;  
					open_flag_m[6] = 1;
//					printf("GIMBAL5 Firmware Found ! Verion = %d type = %s \r\n",find_files[5].version,find_files[5].tail);
				}else if( sscanf(dds,"D200_ESC_v%d.%s",&find_files[6].version,find_files[6].tail) == 2 )
				{
					memcpy(find_files[6].path,path_g,strlen(path_g));
					fw_path[7] = find_files[6].path;  
					open_flag_m[7] = 1;
					fw_path[8] = find_files[6].path;  
					open_flag_m[8] = 1;
					fw_path[9] = find_files[6].path;  
					open_flag_m[9] = 1;
					fw_path[10] = find_files[6].path;  
					open_flag_m[10] = 1;
//					printf("ESC Firmware Found ! Verion = %d type = %s \r\n",find_files[6].version,find_files[6].tail);
				}else if( sscanf(dds,"D200_DRONE_v%d.%s",&find_files[7].version,find_files[7].tail) == 2 )
				{
					memcpy(find_files[7].path,path_g,strlen(path_g));
					fw_path[11] = find_files[7].path;  
					open_flag_m[11] = 1;
//					printf("DRONE Firmware Found ! Verion = %d type = %s \r\n",find_files[7].version,find_files[7].tail);
				}else if( sscanf(dds,"D200_RX1_v%d.%s",&find_files[8].version,find_files[8].tail) == 2 )
				{
					memcpy(find_files[8].path,path_g,strlen(path_g));
					fw_path[12] = find_files[8].path;  
					open_flag_m[12] = 1;
				}else if( sscanf(dds,"D200_VIDEO_v%d.%s",&find_files[9].version,find_files[9].tail) == 2 )
			    {
					memcpy(find_files[9].path,path_g,strlen(path_g));
					fw_path[13] = find_files[9].path;  
					open_flag_m[13] = 1;
				}else if( sscanf(dds,"Radio_App_V%d.%s",&find_files[10].version,find_files[10].tail) == 2 )
			    {
					memcpy(find_files[10].path,path_g,strlen(path_g));
					fw_path[14] = find_files[10].path;  
					open_flag_m[14] = 1;
				}
				else
				{
//					printf("Unkown file: %s\r\n",dds);
				}
            }
    }
	/*------------------------------*/
    tempFind.Close();
	/*---------------*/
    return 0;
}
/*--------------------------------*/
int _tmain(int argc, _TCHAR* argv[])
{
	if( argc == 1 )
	{
		printf("param error %d\r\n",argc);
		/*------------------*/
		return (-1);
	}
	/*------------------*/
  	for(int i = 1;i<argc;i++)
	{
       Tchar_to_char(argv[i],name_buffer[i-1]);
	}
	/*------------------------------------------------------------*/
    if( strcmp(name_buffer[0],"merge") == 0 )
	{
		if( strcmp(name_buffer[1],"-path") == 0 )
		{
	
		}else
		{
			printf("COMMAND:[merge] has not [%s] \r\n",name_buffer[1]);
			return (-1);
		}
	}else if( strcmp(name_buffer[0],"--help") == 0 )
	{
		printf("nothing for help!\r\n");
		return (-1);
	}
	else
	{
		printf("can not found : %s \r\n" , name_buffer[0]);
		return (-1);
	}
	/*---------------*/
	USES_CONVERSION;
	/*---------------*/
	current_path = A2T(name_buffer[3]);//_T("D:\\D200\\D200_release_version\\firmware_collection");

    FindBmpFile(current_path);

	/* check file */
	Merge_process();
	/*------------------------------*/
	return 0;
}
/* merge time */
void Merge_process()
{
	char name_buffer_fm[500];
	static char buffer[500];
	char * file_name = NULL;
	head_info info;
	unsigned short config_global = 0xAA55;
	/* info init */
	int mi = 0,len;
	unsigned int ld;
	info_init(&info);
	/* write addr */
	write_addr = info.nrs_info.fw_start_addr;
	/*----------*/
	USES_CONVERSION;
	/* copy */
	for( int i = 0 ; i < 15 ; i ++ )
	{
	   /* has not open */
	   if( open_flag_m[i] == 0 )
	   {
		   continue;
	   }
	   file_name = fw_path[i];
	   /* open */
	   fopen_s(&file[i],file_name,"rb");
	   /*-----------------*/
       if( file[i] != NULL )
	   {
		   if( i == 12 || i == 13 || i == 14 )
		   {
			   info.modules_info[mi].index = i + 7;
		   }else
		   {
		       info.modules_info[mi].index = i;
		   }
		   memcpy(&info.modules_info[mi].logo,logo_index[i],16);
		   info.modules_info[mi].offet = write_addr;
           /* get file size */
           fseek(file[i],0,SEEK_END);
		   info.modules_info[mi].size = ftell(file[i]);
		   fseek(file[i],0,SEEK_SET);
		   /* file size overflow */
		   if( info.modules_info[mi].size > 2 * 1024 * 1024 )
		   {
		      printf("firmware size over 2MB\r\n%s\r\n",file_name);
			  return;
		   }
		   /* ok , number sector used */
		   info.modules_info[mi].sector_num = (info.modules_info[mi].size / 512) + ((info.modules_info[mi].size % 512 )?1:0);
		   /* get num */
           info.modules_info[mi].config = config_global;
		   /* get check */
		   info.modules_info[mi].check = 0;
		   /* copy data */
		   ld = info.modules_info[mi].offet;
		   /*----------------*/
		   while(1)
		   {
			    /* read */
                len = fread(read_buffer,1,sizeof(read_buffer),file[i]);
                /*-------------------*/
				memcpy(&write_buffer[ld],read_buffer,len);
				/* get version */
				info.modules_info[mi].version = get_version(read_buffer,len,i);
				/*========================*/
				ld += len;
				/*+++++++++++++++++*/
				if( len < sizeof(read_buffer) )
				{
					/* read succeed */
					fclose(file[i]);
                    write_addr += info.modules_info[mi].sector_num * 512;
					mi++;
				    break;
				}
		   }
	   }
	}
	/* number of modules */
	info.nrs_info.numfw = mi;
	/* fw_size */
	info.nrs_info.fw_size = write_addr;
	/* check cal */
	info.nrs_info.check = check_cal( write_buffer + info.nrs_info.fw_start_addr , write_addr - info.nrs_info.fw_start_addr);
	/*----------*/
    info.check_nrs = check_cal( (unsigned char *)&info , sizeof(info) - 2 );
    /* write_buffer */
	memcpy(write_buffer,&info,sizeof(info));
	/* open ? */
	if( file_name == NULL )
	{
		printf("Can not find firmware files\r\n");
		return;
	}
    /* merge ok .then create the sunfile */
	if( FM_file_name(name_buffer_fm,name_buffer[2],find_files[7].version) != 0 )
	{
		printf("Create .FM file fail\r\n");
		return;
	}
    fopen_s(&sunFile,name_buffer_fm,"wb+");
	/*dd*/
	if( sunFile == NULL )
	{
		printf("Create file error\r\n");
		return;
	}
	/* ok */
	fwrite(write_buffer,1,write_addr,sunFile);//fwrite(file_buffer,1,len,fp_create);
	fclose(sunFile);
	/*--------------*/
	memset(buffer,0,sizeof(buffer));
    /*  */
	for( unsigned int i = 0; i < info.nrs_info.numfw ; i ++ )
	{
		sprintf_s(buffer+strlen(buffer),sizeof(buffer),"|%16s size:%8d offet:0x%06X version: %5d | \r\n",info.modules_info[i].logo,info.modules_info[i].size,info.modules_info[i].offet,info.modules_info[i].version);
	}
	/*----------------------------*/
	printf("+-------------------------------------------------------------+\r\n");
    /* transfer */
    printf(buffer);
	printf("+-------------------------------------------------------------+\r\n");
	/*---------------------*/
	printf("OK ! fm_size %d info_check: 0x%X NRS: 0x%X  num: %d\r\n",write_addr,info.nrs_info.check,info.check_nrs,info.nrs_info.numfw);
	/* get */
}

void info_init( head_info * info )
{
    memset(info,0xff,sizeof(head_info));
    /* logo */
	memcpy(info->nrs_info.logo,"%<feimarobotics>",16);
	/* tail */
	info->nrs_info.fw_start_addr = sizeof(head_info);
}

unsigned check_cal(unsigned char * data , unsigned int len)
{
	unsigned int sum = 0 ,i = 0;
	
	for( i = 0; i < len ; i ++ )
	{
		sum += data[i];
	}

	return sum;
}

unsigned short get_version(unsigned char * data,unsigned int len , unsigned int index )
{
	if( index != 14 )
	{
		unsigned int *p = (unsigned int *)data;
    
		for( unsigned int i = 0 ; i < len ; i ++ )
		{
			/* --------------- */
			p = (unsigned int *)&data[i];
			/* --------------- */
			if( p[0] == version_export[0] && p[2] == version_export[2] )
			{
				/* good we get the version */
				return p[1] & 0xffff;
			}
		}
		/* error */
		return 0;
	}else
	{
		/* radio */
		unsigned char  ver_id[10] = {0x46,0x31,0x30,0x30,0x30,0x5f,0x72,0x64,0x5f,0x76};
		/*----------------------*/
		int i = 0;
		/*----------------------*/
		while( i < len )
		{
			int j = 0;
			/*--------------------------*/
			for( j = 0 ; j < 10 ; j ++ )
			{
				if( data[i+j] != ver_id[j] )
				{
					break;
				}
			}
			/*-----------------------------*/
			if( j == 10 )
			{
				/* ok find */
				unsigned short ver = ( data[i+10] - 0x30 ) * 100 + ( data[i+12] - 0x30 ) * 10 + ( data[i+14] - 0x30 );
				/* return */
				return ver;
			}
			/*-----------------------------*/
			i++;
		}
		/* error */
		return 0;
	}
}
/* get fm file name */
int FM_file_name(char * name_buffer,const char * path,unsigned short fc_version)
{
   /*------------------*/
   sprintf_s(name_buffer,500,"%s//D200_Autopilot_v%d.fm",path,fc_version);
   /* return ok */
   return 0;
}

int Tchar_to_char(_TCHAR * tchar,char * buffer)
{
    int i = 0;
	memset(buffer,0,sizeof(buffer));

	while(*tchar != '\0')
	{
       buffer[i] = (char)(*tchar);
	   tchar ++;
	   i++;
	}
	return i;
}