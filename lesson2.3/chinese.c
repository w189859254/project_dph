/* example1.c                                                      */
/*                                                                 */
/* This small program shows how to print a rotated string with the */
/* FreeType 2 library.                                             */


#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H

/* 假设屏幕分辨率是640*480 */
#define WIDTH   80
#define HEIGHT  80

/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];

/* Replace this function with something useful. */
//把点阵存在数组里面
void
draw_bitmap( FT_Bitmap*  bitmap,
             FT_Int      x,
             FT_Int      y)
{
	FT_Int  i, j, p, q;
  	FT_Int  x_max = x + bitmap->width;
  	FT_Int  y_max = y + bitmap->rows;


  	for ( i = x, p = 0; i < x_max; i++, p++ )
  	{
    	for ( j = y, q = 0; j < y_max; j++, q++ )
    	{
      		if ( i < 0      || j < 0       ||
           	i >= WIDTH || j >= HEIGHT )
        		continue;

      		image[j][i] |= bitmap->buffer[q * bitmap->width + p];
    	}
  	}
}


void
show_image( void )
{
  	int  i, j;


  	for ( i = 0; i < HEIGHT; i++ )
  	{
    	for ( j = 0; j < WIDTH; j++ )
      	putchar( image[i][j] == 0 ? ' '
                                : image[i][j] < 128 ? '+'
                                                    : '*' );
    	putchar( '\n' );
  	}
}


int
main( int     argc,
      char**  argv )
{
  	FT_Library    library;
  	FT_Face       face;

  	FT_GlyphSlot  slot;
  	FT_Matrix     matrix;                 /* transformation matrix */
  	FT_Vector     pen;                    /* untransformed origin 原点位置 */ 
  	FT_Error      error;

  	char*         filename;
  	char*         text;

  	double        angle;
  	int           target_height;
  	int           n, num_chars;
/*显示中文*/
	int chinese_str[] = {0x6587, 0x6d69, 0x0067 };

  	if ( argc != 3 )
  	{
    	fprintf ( stderr, "usage: %s font sample-text\n", argv[0] );
    	exit( 1 );
  	}

  	filename      = argv[1];                           /* first argument     */
  	text          = argv[2];                           /* second argument    */
  	num_chars     = strlen( text );
  	angle         = ( 0.0 / 360 ) * 3.14159 * 2;      /* use 0 degrees     */
  	target_height = HEIGHT;

  	error = FT_Init_FreeType( &library );              /* initialize library */
  	/* error handling omitted */

  	error = FT_New_Face( library, argv[1], 0, &face ); /* create face object */
  	/* error handling omitted */
#if 0
  	/* use 50pt at 100dpi */
  	error = FT_Set_Char_Size( face, 50 * 64, 0,
                            100, 0 );                /* set character size */                         
   	/*pixels = 50*64 / 64 / 72 *100 = 69 ,*/                        
#else /*对于LCD只管它的像素大小,0表示高度==宽度,24*24的点阵大小*/
	FT_Set_Pixel_Sizes(face, 24, 0);
#endif                          
                            
  	/* error handling omitted */

  	slot = face->glyph; /* 把图像存在这里 */

  	/* set up matrix 用来旋转这个字体的*/ 
  	matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
  	matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
  	matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
  	matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

  	/* the pen position in 26.6 cartesian space coordinates; */
  	/* start at (0,40) relative to the upper left corner  */
  	pen.x = 0 * 64; /*它们的单位是1/64,所以 *64*/
  	pen.y = ( target_height - 40 ) * 64;

  	for ( n = 0; n < num_chars; n++ )
  	{
    	/* set transformation */
    	FT_Set_Transform( face, &matrix, &pen );

    	/* load glyph image into the slot (erase previous one) */
    	error = FT_Load_Char( face, chinese_str[n], FT_LOAD_RENDER );/*得到字符的点阵*/
    	if ( error )
      		continue;                 /* ignore errors */

    	/* now, draw to our target surface (convert position) */
    	draw_bitmap( &slot->bitmap,
                 slot->bitmap_left, /*点阵的x坐标*/
                 target_height - slot->bitmap_top );/*LCD的高度-笛卡尔坐标=LCD认为的Y坐标*/

    /* increment pen position */
    	pen.x += slot->advance.x;
    	pen.y += slot->advance.y;
  	}

  	show_image();

  	FT_Done_Face    ( face );
  	FT_Done_FreeType( library );

  	return 0;
}

/* EOF */
