// FINDX.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "afx.h"
#include <stdio.h>
#include <winsock.h>
#include "merge_msg.h"

static char fw_path[24][200];

static unsigned char open_flag_m[24];

static unsigned char write_buffer[ 1024 * 1024 * 20 ]; // max to 20 MByte
static unsigned int write_addr = 0;

static unsigned char read_buffer[ 2 * 1024 * 1024 ];// 2 MByte

const unsigned int version_export[3] = {0xeabc2547,0,0x3526ec88};

static unsigned short fc_version = 0xffff;

CString dis;
static FILE * file[24];
static FILE * sunFile;

/*---------------------------*/
int change_version(const char * path , const char * version_ascill);
int get_file_version(char *bss,char * path);
void Merge_process();
void info_init( head_info * info );
unsigned check_cal(unsigned char * data , unsigned int len);
unsigned short get_version(unsigned char * data,unsigned int len , unsigned int index );
int FM_file_name(char * name_buffer,const char * path,unsigned short fc_version);
/*---------------------------*/
const char logo_index[24][16] = {
	"gps board",//0
    "flight_loader",//1
    "AIRSPEED",//2
    "esc1",//3
	"esc2",//4
    "esc3",//5
	"bottom board",//6
	"CAN_R1",//7
    "CAN_R2",//8
    "CAN_R3",//9
    "mag1",//10
    "mag2",//11
    "CAN_M01",//12
    "CAN_M02",//13
    "CAN_M03",//14
    "rev1",//15
    "rev2",//16
    "rev3",//17
    "rev4",//18
    "rev5",//19
    "rev6",//20
    "radio",//21
    "rev7",//22
	"fc",//23
};//24

unsigned msg_version;
char msg_tail[16];
char  msg_path[256];

static char name_buffer[128][200];

//#pragma  comment(  linker,  "/subsystem:\"windows\"  /entry:\"wmainCRTStartup\""  )

unsigned int file_find_cnt = 0,file_add_cnt = 0;

char * dds,*path_g;

CString current_path;

static unsigned int file_flags = 0;

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
				/* get version and file */
				get_file_version(dds,path_g);
            }
    }
	/*------------------------------*/
    tempFind.Close();
	/*---------------*/
    return 0;
}
/* int get file version */
int get_file_version(char *dds,char * path_g)
{
	/* case the file name */
	if( sscanf(dds,"V100_GPS_v%d.%s",&msg_version,msg_tail) == 2 )
	{		
		open_flag_m[0] = 1;
		memcpy(fw_path[0],path_g,strlen(path_g));
	}
	else if( sscanf(dds,"V100_CAM_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[1] = 1;
		memcpy(fw_path[1],path_g,strlen(path_g));
	}
	else if( sscanf(dds,"V100_ESC_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[3] = 1;
		memcpy(fw_path[3],path_g,strlen(path_g));
		open_flag_m[4] = 1;
		memcpy(fw_path[4],path_g,strlen(path_g));
		open_flag_m[5] = 1;
		memcpy(fw_path[5],path_g,strlen(path_g));
	}
	else if( sscanf(dds,"V100_BOTTOM_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[6] = 1;
		memcpy(fw_path[6],path_g,strlen(path_g));
	}
	else if( sscanf(dds,"V100_R_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[7] = 1;
		memcpy(fw_path[7],path_g,strlen(path_g));
		open_flag_m[8] = 1;
		memcpy(fw_path[8],path_g,strlen(path_g));
		open_flag_m[9] = 1;
		memcpy(fw_path[9],path_g,strlen(path_g));
	}
	else if( sscanf(dds,"V100_M_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[12] = 1;
		memcpy(fw_path[12],path_g,strlen(path_g));
		open_flag_m[13] = 1;
		memcpy(fw_path[13],path_g,strlen(path_g));
		open_flag_m[14] = 1;
		memcpy(fw_path[14],path_g,strlen(path_g));
		open_flag_m[10] = 1;
		memcpy(fw_path[10],path_g,strlen(path_g));
		open_flag_m[11] = 1;
		memcpy(fw_path[11],path_g,strlen(path_g));
		open_flag_m[2] = 1;
		memcpy(fw_path[2],path_g,strlen(path_g));
//		printf("SONAR Firmware Found ! Verion = %d type = %s \r\n",find_files[3].version,find_files[3].tail);
	}else if( sscanf(dds,"V100_REV1_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[15] = 1;
		memcpy(fw_path[15],path_g,strlen(path_g));
	}
	else if( sscanf(dds,"V100_REV2_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[16] = 1;
		memcpy(fw_path[16],path_g,strlen(path_g));
	}else if( sscanf(dds,"V100_REV3_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[17] = 1;
		memcpy(fw_path[17],path_g,strlen(path_g));
	}else if( sscanf(dds,"V100_REV4_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[18] = 1;
		memcpy(fw_path[18],path_g,strlen(path_g));
	}else if( sscanf(dds,"V100_REV5_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[19] = 1;
		memcpy(fw_path[19],path_g,strlen(path_g));
	}else if( sscanf(dds,"V100_REV6_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[20] = 1;
		memcpy(fw_path[20],path_g,strlen(path_g));
	}else if( sscanf(dds,"Radio_App_V%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[21] = 1;
		memcpy(fw_path[21],path_g,strlen(path_g));
	}else if( sscanf(dds,"V100_REV7_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[22] = 1;
		memcpy(fw_path[22],path_g,strlen(path_g));
	}else if( sscanf(dds,"V100_DRONE_v%d.%s",&msg_version,msg_tail) == 2 )
	{
		open_flag_m[23] = 1;
		memcpy(fw_path[23],path_g,strlen(path_g));
	}
	else
	{
		printf("Unkown file: %s\r\n",path_g);
		return (-1);
	}
	/*------------------------*/
	return 0;
}
int get_name(const char * path,char * buffer)
{
	int len = strlen(path);

	unsigned int pos = 0;

	for( int i = 0 ; i < len ; i ++ )
	{
		if( path[i] == '\\' )
		{
			pos = i;
		}
	}
	/* memcpy */
	if( pos )
	{
		memcpy( buffer , &path[pos+1] , len - pos - 1);
		/*---------------------------------*/
		return 0;
	}
	/*----------------------------*/
	return (-1);
}
/*--------------------------------*/
int _tmain(int argc, _TCHAR* argv[])
{
	if( argc == 1 )
	{
		printf("+-------------------------------------+\r\n");
		printf("|  merge tools at console v0.1.7      |\r\n");
		printf("|  How to use? I don't know either .  |\r\n");
		printf("|  [--help] maybe useful.             |\r\n");
		printf("+-------------------------------------+\r\n");
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
	else if( strcmp(name_buffer[0],"--version") == 0 )
	{
		printf("v0.1.7_build20181218\r\n");
		return (-1);
	}else if( strcmp(name_buffer[0],"-f") == 0 )
	{
		if( argc > 4 )
		{
			/* decode */
			for( int i = 0 ; i < argc - 4 ; i ++ )
			{
				char name_tmp[200];
				/*-----------------------------------*/
				memset(name_tmp,0,sizeof(name_tmp));
				/*-----------------------------------*/
				if( get_name(name_buffer[3+i],name_tmp) == 0 )
				{
					/*-----------------------------------*/
					//printf("-f ok %s %s\r\n",name_buffer[3+i] , name_tmp);
					/* do something */
					if( get_file_version(name_tmp,name_buffer[3+i]) != 0 )
					{
						return (-1);
					}
					/*-----------------------------------------------*/
				}else
				{
					printf("invail path  : %s \r\n" , name_buffer[3+i]);
				    return (-1);
				}
			}
		}else
		{
			printf("-f error %d\r\n",argc);
			printf("[-f] [option] [path] [file] \r\n");
			return (-1);
		}
		/* exit */
		//printf("-f exit\r\n");
		/*---------*/
		file_flags = 1;
	}
	else if( strcmp(name_buffer[0],"-c") == 0 )
	{
		if( argc == 5 )
		{
			/*-----------------------------------*/
			change_version(name_buffer[3],name_buffer[2]);
			/*-----------------------------------*/
		}else
		{
			printf("[-c] [option] [version] [file]\r\n");
		}
		/* chang name */
		/*------------------------------*/
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
	if( !file_flags )
	{
		current_path = A2T(name_buffer[2]);//_T("D:\\D200\\D200_release_version\\firmware_collection");

		FindBmpFile(current_path);
	}
	/* check file */
	Merge_process();
	/*------------------------------*/
	return 0;
}
/* merge time */
void Merge_process()
{
	int xf_flag = 0;
	time_t t = time(0);
	/*------------------*/
	if( t > 1554048000 )//2019-4-1 00:00:00
	{
       xf_flag = 1;
	}
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
	for( int i = 0 ; i < 24 ; i ++ )
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
		  
		   info.modules_info[mi].index = i;
	
		   memcpy(&info.modules_info[mi].logo,logo_index[i],16);
		   info.modules_info[mi].offet = write_addr;
           /* get file size */
           fseek(file[i],0,SEEK_END);
		   info.modules_info[mi].size = ftell(file[i]);
		   fseek(file[i],0,SEEK_SET);
		   /* liscence */
		   if( xf_flag == 1 && i == 23 )
		   {
			   info.modules_info[mi].size -= 10*1024;
		   }
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
				/*------------------------*/
				if( i == 23 )
				{
					fc_version = info.modules_info[mi].version;
				}
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
	if( FM_file_name(name_buffer_fm,name_buffer[2],fc_version) != 0 )
	{
		printf("Create .FM file fail\r\n");
		return;
	}
    fopen_s(&sunFile,name_buffer_fm,"wb+");
	/*dd*/
	if( sunFile == NULL )
	{
		printf("Create file error %s\r\n",name_buffer_fm);
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
	if( index != 21 )
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
   sprintf_s(name_buffer,500,"%s//V100_Autopilot_v%d.fm",path,fc_version);
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
/*-----------------------------------------------*/
int change_version(const char * path , const char * version_ascill)
{
	unsigned short version = 0;
	unsigned int * ver;
	/*----------------------*/
	if( sscanf(version_ascill,"%d",&version) != 1 )
	{
		printf("invail version %d , %s \r\n",version,version_ascill);
		return (-1);
	}
	/*---------------------*/
	FILE * rb = fopen(path,"rb");
	/*------------------------*/
	if( rb == NULL )
	{
		printf("ERROR---->file not exist %s\r\n",path);
		return (-1);
	}
	/*------------------------------*/
	int len = fread(read_buffer,1,sizeof(read_buffer),rb);
	/*------------------------------*/
	fclose(rb);
	/*-------------------------------------------------*/
	if( len == sizeof(read_buffer))
	{
		printf("file size maybe over 2MB\r\n");
		return (-1);
	}
	/*------------------------*/
	int i = 0;
	/*------------------------------------------------*/
	for( i = 0 ; i < len ; i +=4 )
	{
		ver = (unsigned int *)&read_buffer[i];
		/*----------------------------------*/
		if( ver[0] == version_export[0] && ver[2] == version_export[2] )
		{
			/* get */
			ver[1] = version;
			/*------------------*/
			break;
		}
	}
	/*------------------------------------------------*/
	if( i < len )
	{
		/*---------------------------------*/
		FILE * wb = fopen(path,"wb+");
		/*---------------------------------*/
		if( wb == NULL )
		{
			printf("file creater fail %s\r\n",path);
			return (-1);
		}
		/*----------------------------------*/
		fwrite(read_buffer,1,len,wb);
		/*-------------------------*/
		fclose(wb);
		/*----------------------------------*/
		printf("ok changed to %d\r\n",version);
		/*----------------------------------*/
		return 0;
	}else
	{
		/*---------------------------------*/
		printf("Can not find version id\r\n");
		/*---------------------------------*/
		return (-1);
	}
	/*------------------------------------------------*/
	return 0;
}