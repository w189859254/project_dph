#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <draw.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <string.h>


/* ./show_file [-s Size] [-f freetype_font_file] [-h HZK] <text_file> */
int main(int argc, char **argv)
{
	int iError;
	unsigned int dwFontSize = 16;
	char acHzkFile[128];
	char acFreetypeFile[128];
	char acTextFile[128];

	char acDisplay[128];

	char cOpr;
	int bList = 0;

	acHzkFile[0]  = '\0';
	acFreetypeFile[0] = '\0';
	acTextFile[0] = '\0';

	/* Ŀǰֻ֧��fb */
	strcpy(acDisplay, "fb");
	
	/* s�������:��ʾ������в���,��ĸlҲ�Ǹ�ѡ�
     * ����û�д���ð�ţ���ʾ��û�в���������û�н���������--
	 * text_file�����argv[optind]����
	 */
	while ((iError = getopt(argc, argv, "ls:f:h:d:")) != -1)
	{
		/* ѡ�� */
		switch(iError)
		{
			case 'l':
			{
				  bList = 1;
				  break;
			}
			case 's':
			{
				  dwFontSize = strtoul(optarg, NULL, 0); /*��ѡ���������optarg����*/
				  break;
			}
			case 'f':
			{
				  strncpy(acFreetypeFile, optarg, 128);
				  acFreetypeFile[127] = '\0';
				  break;
			}			
			case 'h':
			{
					strncpy(acHzkFile, optarg, 128);
					acHzkFile[127] = '\0';
					break;
			}
			case 'd':
			{
				strncpy(acDisplay, optarg, 128);
				acDisplay[127] = '\0';
				break;
			}
			default:
			{
					printf("Usage: %s [-s Size] [-d display] [-f font_file] [-h HZK] <text_file>\n", argv[0]);
					printf("Usage: %s -l\n", argv[0]);
					return -1;
					break;
			}
		}
	}

	/* optindex,text_file�����argv[optind],�����text_file����������ض�
	 * optind < argc
	*/
	if (!bList && (optind >= argc))
	{
		printf("Usage: %s [-s Size] [-d display] [-f font_file] [-h HZK] <text_file>\n", argv[0]);
		printf("Usage: %s -l\n", argv[0]);
		return -1;
	}
		

	if (bList)
	{
		printf("supported display:\n");
		ShowDispOpr();

		printf("supported font:\n");
		ShowFontOpr();

		printf("supported encoding:\n");
		ShowEncodingOpr();
		return 0;
	}

	strncpy(acTextFile, argv[optind], 128);
	acTextFile[127] = '\0';

	iError = DrawInit();
	if (iError)
	{
		printf("DrawInit error!\n");
		return -1;
	}
	
	/* ���ļ�ӳ�䵽�ڴ棬Ϊ�ļ�ѡ������� */
	iError = OpenTextFile(acTextFile);
	if (iError)
	{
		printf("OpenTextFile error!\n");
		return -1;
	}
	
	/* ���ݾ�����ı��ļ����г�ʼ�� */
	iError = SetFontsDetail(acHzkFile, acFreetypeFile,dwFontSize);
	if (iError)
	{
		printf("SetFontsDetail error!\n");
		return -1;
	}

	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	/* ѡ��ͳ�ʼ����ʾ�� */
	iError = SelectAndInitDisplay(acDisplay);
	if (iError)
	{
		printf("SelectAndInitDisplay error!\n");
		return -1;
	}
/* ǰ���Ѿ�Ϊ�ı��ļ����ú��˱�������������Ϣ��ѡ����ʾ�� */


	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	iError = ShowNextPage(); /*��һҳ*/
	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	if (iError)
	{
		printf("Error to show first page\n");
		return -1;
	}

	while (1)
	{
		printf("Enter 'n' to show next page, 'u' to show previous page, 'q' to exit: ");

		do {
			cOpr = getchar();			
		} while ((cOpr != 'n') && (cOpr != 'u') && (cOpr != 'q'));

		if (cOpr == 'n')
		{
			ShowNextPage();
		}
		else if (cOpr == 'u')
		{
			ShowPrePage();			
		}
		else 
		{
			return 0;
		}			
	}
	return 0;
}

