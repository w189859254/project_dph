#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

static int FreeTypeFontInit(char * pcFontFILE, unsigned int dwFontSize);
static int FreeTypeGetFontBitmap(unsigned int dwcode, PT_FontBitMap ptFontBitMap);

static FT_Library 	g_tLibrary;
static FT_Face 	  	g_tFace;
static FT_GlyphSlot g_tSlot;

/*分配、设置、注册T_FontOpr*/
static T_FontOpr g_tFreeTypeFontOpr = {
	.name 			= "freetype",
	.FontInit 		= FreeTypeFontInit,
	.GetFontBitmap 	= FreeTypeGetFontBitmap,
};


static int FreeTypeFontInit(char * pcFontFILE, unsigned int dwFontSize)
{
	int iError;
	/* 显示矢量字体 */
	iError = FT_Init_FreeType( &g_tLibrary );      /* 初始化库 */
	if (iError)
	{
		DBG_PRINT("FT_Init_FreeType error!\n");
		return -1;

	}

  	iError = FT_New_Face( g_tLibrary, argv[1], 0, &g_tFace); /* 打开字体文件 */
  	if (iError)
	{
		DBG_PRINT("FT_New_Face error!\n");
		return -1;

	}

	g_tSlot = g_tFace->glyph;

	iError = FT_Set_Pixel_Sizes(g_tFace, dwFontSize, 0);		/*设置大小*/
  	if (iError)
	{
		DBG_PRINT("FT_Set_Pixel_Sizes error!\n");
		return -1;
	}
	return 0;
}


static int FreeTypeGetFontBitmap(unsigned int dwcode, PT_FontBitMap ptFontBitMap)
{
	int iError;
	int iPenX = ptFontBitMap->iCurOriginX;
	int iPenY = ptFontBitMap->iCurOriginY;
	
	/* load glyph image into the slot (erase previous one) */
	/* FT_LOAD_RENDER 表示loder的时候把原来的矢量文件转为位图
	 * FT_LOAD_MONOCHROME 转为单色位图
	*/
	iError = FT_Load_Char( g_tFace, dwcode, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
	if (iError)
	{
		DBG_PRINT("FT_Load_Char error\n");
		return -1;
	}

	ptFontBitMap->iXLeft       = iPenX + g_tSlot->bitmap_left;
	ptFontBitMap->iYTop        = iPenY - g_tSlot->bitmap_top;
	ptFontBitMap->iXMax        = ptFontBitMap->iXLeft + g_tSlot->bitmap.width;
	ptFontBitMap->iYMax        = ptFontBitMap->iYTop  + g_tSlot->bitmap.rows;
	ptFontBitMap->iBpp	   	   = 1;
	ptFontBitMap->iPitch       = g_tSlot->bitmap.pitch;
	ptFontBitMap->iNextOriginX = iPenX + g_tSlot->advance.x / 64;
	ptFontBitMap->iNextOriginY = iPenY;//在同一行画，y保持不变
	ptFontBitMap->pucBuffer    = g_tSlot->bitmap.buffer;

	return 0;
}

int FreeTypeInit(void)
{
	return RegisterFontOpr(&g_tFreeTypeFontOpr);
}































































