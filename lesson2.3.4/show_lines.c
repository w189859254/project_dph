#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <math.h>
#include <wchar.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

int fd_fb;
struct fb_var_screeninfo var;	/* Current var */
struct fb_fix_screeninfo fix;	/* Current fix */
int screen_size;
unsigned char *fbmem;
unsigned int line_width;
unsigned int pixel_width;


/* color : 0x00RRGGBB */
void lcd_put_pixel(int x, int y, unsigned int color)
{
	unsigned char *pen_8 = fbmem+y*line_width+x*pixel_width;
	unsigned short *pen_16;	
	unsigned int *pen_32;	

	unsigned int red, green, blue;	

	pen_16 = (unsigned short *)pen_8;
	pen_32 = (unsigned int *)pen_8;

	switch (var.bits_per_pixel)
	{
		case 8:
		{
			*pen_8 = color;
			break;
		}
		case 16:
		{
			/* 565 */
			red   = (color >> 16) & 0xff;
			green = (color >> 8) & 0xff;
			blue  = (color >> 0) & 0xff;
			color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			*pen_16 = color;
			break;
		}
		case 32:
		{
			*pen_32 = color;
			break;
		}
		default:
		{
			printf("can't surport %dbpp\n", var.bits_per_pixel);
			break;
		}
	}
}


void
draw_bitmap( FT_Bitmap*  bitmap,
             FT_Int      x,
             FT_Int      y)
{
	FT_Int  i, j, p, q;
  	FT_Int  x_max = x + bitmap->width;
  	FT_Int  y_max = y + bitmap->rows;
  	
//	printf("x = %d, y = %d\n", x, y);

  	for ( i = x, p = 0; i < x_max; i++, p++ )
  	{
    	for ( j = y, q = 0; j < y_max; j++, q++ )
    	{
      		if ( i < 0      || j < 0       ||
           	i >= var.xres || j >= var.yres )
        		continue;

      		//image[j][i] |= bitmap->buffer[q * bitmap->width + p];//buffer里面存的是点的数据
      		lcd_put_pixel(i, j, bitmap->buffer[q * bitmap->width + p]);
    	}
  	}
}


int main(int argc, char **argv)
{
	wchar_t *wstr1 = L"文浩大帅比gif";
	wchar_t *wstr2 = L"www.baidu.com";
	
	FT_Library 	library;
	FT_Face 	face;
	int error;
	FT_Vector pen;
  	FT_GlyphSlot  slot;
	int i;
	FT_BBox bbox;
	FT_Glyph glyph;

	int line_box_ymin = 10000;
	int line_box_ymax = 0;

	if (argc != 2)
	{
		printf("Usage: %s <font_file> <angle>\n", argv[0]);
		return -1;
	}

	fd_fb = open("/dev/fb0", O_RDWR);
	if (fd_fb < 0)
	{
		printf("can't open /dev/fb0\n");
		return -1;
	}

	if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &var))
	{
		printf("can't get var\n");
		return -1;
	}

	if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &fix))
	{
		printf("can't get fix\n");
		return -1;
	}

	line_width  = var.xres * var.bits_per_pixel / 8;
	pixel_width = var.bits_per_pixel / 8;
	screen_size = var.xres * var.yres * var.bits_per_pixel / 8;
	fbmem = (unsigned char *)mmap(NULL , screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
	if (fbmem == (unsigned char *)-1)
	{
		printf("can't mmap\n");
		return -1;
	}

	/* 清屏: 全部设为黑色 */
	memset(fbmem, 0, screen_size);

/* 显示矢量字体 */
	error = FT_Init_FreeType( &library );              /* 初始化库 */
  	/* error handling omitted */

  	error = FT_New_Face( library, argv[1], 0, &face ); /* 打开字体文件 */
  	/* error handling omitted */
	/*装载一个字体的时候，字体数据就会放到插槽里面中，加载一次，slot
	就变化一次，slot里面的是最新的数据*/
	slot = face->glyph;

  	FT_Set_Pixel_Sizes(face, 24, 0);					/*设置大小*/

  	/* 确定坐标,假设文是24*24
  	 * lcd_x = 0
	 * lcd_y = 24
	 * 笛卡尔坐标系:
	 * x = lcd_x = 0
	 * y = var.yres - lcd_y = var.yres/2 - 24
  	*/
	pen.x = 0 * 64;
  	pen.y = ( var.yres - 24 ) * 64; //原点

	for (i = 0; i < wcslen(wstr1); i++)
	{
		/* set transformation */
	    FT_Set_Transform( face, 0, &pen );//若不想让字体旋转角度，矩阵可以不写。

	    /* load glyph image into the slot (erase previous one) */
	    /*FT_LOAD_RENDER 表示loder的时候把原来的矢量文件转为位图*/
	    error = FT_Load_Char( face, wstr1[i], FT_LOAD_RENDER );
		if (error)
		{
			printf("FT_Load_Char error\n");
			return -1;
		}

		error = FT_Get_Glyph( face->glyph, &glyph );
		if (error)
		{
			printf("FT_Get_Glyph error!\n");
			return -1;
		}
		/*得到bbox,里面含有每个文字的位置信息*/
		FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_TRUNCATE, &bbox);
		
		/*这些值都是笛卡尔坐标系*/
		if (line_box_ymin > bbox.yMin)   
			line_box_ymin = bbox.yMin;
		if (line_box_ymax < bbox.yMax)
			line_box_ymax = bbox.yMax;

		draw_bitmap( &slot->bitmap,
	                 slot->bitmap_left,
	                 var.yres - slot->bitmap_top );

    	/* increment pen position */
    	pen.x += slot->advance.x;
    	//pen.y += slot->advance.y; /*只在同一行显示没必要*/

	}

	/* 确定坐标,假设24*24 显示第二行
  	 * lcd_x = 0
	 * lcd_y = line_box_ymax - line_box_ymin + 24
	 * 笛卡尔坐标系:
	 * x = lcd_x = 0
	 * y = var.yres - lcd_y = var.yres - (line_box_ymax - line_box_ymin + 24)
  	*/
	pen.x = 0 * 64;
  	pen.y = ( var.yres - (line_box_ymax - line_box_ymin + 24) ) * 64; //文原点

	for (i = 0; i < wcslen(wstr2); i++)
	{
		/* set transformation */
	    FT_Set_Transform( face, 0, &pen );//若不想让字体旋转角度，矩阵可以不写。

	    /* load glyph image into the slot (erase previous one) */
	    /*FT_LOAD_RENDER 表示loder的时候把原来的矢量文件转为位图*/
	    error = FT_Load_Char( face, wstr2[i], FT_LOAD_RENDER );
		if (error)
		{
			printf("FT_Load_Char error\n");
			return -1;
		}

		error = FT_Get_Glyph( face->glyph, &glyph );
		if (error)
		{
			printf("FT_Get_Glyph error!\n");
			return -1;
		}
		/*得到bbox,里面含有每个文字的位置信息*/
		FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_TRUNCATE, &bbox);
		if (line_box_ymin > bbox.yMin) 
			line_box_ymin = bbox.yMin;
		if (line_box_ymax < bbox.yMax)
			line_box_ymax = bbox.yMax;

		draw_bitmap( &slot->bitmap,
	                 slot->bitmap_left,
	                 var.yres - slot->bitmap_top );

    	/* increment pen position */
    	pen.x += slot->advance.x;
    	//pen.y += slot->advance.y;
	}
	return 0;	
}

