#ifndef _FONTS_MANAGER_H
#define _FONTS_MANAGER_H

/*为了兼容三种取点阵的方法，所以这个结构体很复杂*/
typedef struct FontBitMap {
	int iXLeft;
	int iYTop;
	int iXMax;
	int iYMax;
	int iBpp;	  		/*用一位还是一字节来表示一个像素?*/
	int iPitch;   		/* 对于单色位图, 两行象素之间的跨度,单位字节 */
	int iCurOriginX; 	/*这两个参数是传进去的，不用设置*/
	int iCurOriginY;
	int iNextOriginX;
	int iNextOriginY;
	unsigned char *pucBuffer; /*点阵存储的地方*/
}T_FontBitMap, *PT_FontBitMap;

typedef struct FontOpr {
	char *name;
	int (*FontInit)(char *pcFontFile, unsigned int dwFontSize);
	int (*GetFontBitmap)(unsigned int dwCode, PT_FontBitMap ptFontBitMap);
	struct FontOpr *ptNext;
}T_FontOpr, *PT_FontOpr;


int RegisterFontOpr(PT_FontOpr ptFontOpr);
void ShowFontOpr(void);
int FontsInit(void);
int ASCIIInit(void);
int GBKInit(void);
int FreeTypeInit(void);
PT_FontOpr GetFontOpr(char *pcName);

#endif /* _FONTS_MANAGER_H */

