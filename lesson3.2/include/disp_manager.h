#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H


/* 现在只有fb.c，以后会添加crt.c，使用fb的时候，
 * 需要把fb打开做某些初始化,现在fb里面只是注册，
 * 用到的时候再去初始化,所以需要个deviceinit函数
 */
typedef struct DispOpr {
	char *name;
	int iXres;
	int iYres;
	int iBpp;/*每个像素用多少位表示*/
	int (*DeviceInit)(void);
	int (*ShowPixel)(int iPenX, int iPenY, unsigned int dwColor);
	int (*CleanScreen)(unsigned int dwBackColor);
	struct DispOpr *ptNext;
}T_DispOpr, *PT_DispOpr;

int RegisterDispOpr(PT_DispOpr ptDispOpr);
void ShowDispOpr(void);
int DisplayInit(void);
int FBInit(void);

#endif /* _DISP_MANAGER_H */

