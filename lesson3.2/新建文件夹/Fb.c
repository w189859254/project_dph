static int FBDeviceInit(void);
static FBShowPixel(int iPenX, int iPenY, unsigned int dwColor);
static FBCleanScreen(unsigned int dwBackColor);

static int g_iFBFD;
static struct fb_var_screeninfo g_tVar;
static struct fb_var_svreeninfo g_tFix;
static int g_iScreen_Size;
static unsigned char *g_pucFbMem;
static int g_iLineWidth;
static int g_iPixelWidth;

/*构造、设置、注册一个结构体*/

static T_DispOpr g_tFBDispOpr = {
	.name        = "fb",
	.DeviceInit  = FBDeviceInit,
	.ShowPixel   = FBShowPixel,
	.CleanScreen = FBCleanScreen,

};

static int FBDeviceInit(void)
{
	
	g_iFBFD = open(FB_DEVICE_NAME, O_RDWR);//FB_DEVICE_NAME,不应该把代码定死
	if (g_iFBFD < 0)
	{
		DBG_PRINTF("can't open /dev/fb0\n");
		return -1;
	}
	//获得可变参数
	if (ioctl(g_iFBFD, FBIOGET_VSCREENINFO, &g_tVar))
	{
		DBG_PRINTF("can't get var\n");
		return -1;
	}
	//获得固定参数
	if (ioctl(g_iFBFD, FBIOGET_FSCREENINFO, &g_tFix))
	{
		DBG_PRINTF("can't get fix\n");
		return -1;
	}

	g_tFBDispOpr.iXres = g_tVar.xres;
	g_tFBDispOpr.iYres = g_tVar.yres;
	g_tFBDispOpr.iBpp = g_tVar.bits_per_pixel;
	
	g_iScreen_Size = var.xres * var.yres * var.bits_per_pixel / 8;
	g_iLineWidth   = var.xres * var.bits_per_pixel / 8;
	g_iPixelWidth  = var.bits_per_pixel / 8;
	
	g_pucFbMem = (unsigned char *)mmap(NULL , g_iScreen_Size, PROT_READ | PROT_WRITE, MAP_SHARED, g_iFBFD, 0);
	if (g_pucFbMem == (unsigned char *)-1)
	{
		DBG_PRINTF("can't mmap\n");
		return -1;
	}

	return 0;
}

/* dwColor = RRGGBB */
static FBShowPixel(int iPenX, int iPenY, unsigned int dwColor)
{
	unsigned char  *pucPen8 = g_pucFbMem + iPenY*g_iLineWidth + iPenX*g_iPixelWidth;
	unsigned short *pwPen16;	
	unsigned int *pdwPen32;	

	unsigned int red, green, blue;	

	pwPen16 = (unsigned short *)pucPen8;
	pdwPen32 = (unsigned int *)pucPen8;

	switch (g_tFBDispOpr.iBpp)
	{
		case 8:
		{
			*pucPen8 = dwcolor;
			break;
		}
		case 16:
		{
			/* 565 */
			red       = (dwcolor >> 16) & 0xff;
			green     = (dwcolor >> 8) & 0xff;
			blue      = (dwcolor >> 0) & 0xff;
			dwcolor   = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			*pwPen16  = dwcolor;
			break;
		}
		case 32:
		{
			*pdwPen32 = dwcolor;
			break;
		}
		default:
		{
			DBG_PRINTF("can't surport %dbpp\n", var.bits_per_pixel);
			return -1;
			break;
		}
	}

	return 0;
}

static FBCleanScreen(unsigned int dwBackColor)
{
	unsigned char  *pucPen8 = g_pucFbMem;
	unsigned short *pwPen16;	
	unsigned int *pdwPen32;	
	
	unsigned int red, green, blue;	
	int i;

	pwPen16 = (unsigned short *)pucPen8;
	pdwPen32 = (unsigned int *)pucPen8;

	switch (g_tFBDispOpr.iBpp)
	{
		case 8:
		{
			memset(pucPen8, dwBackColor, g_iScreen_Size);
			break;
		}
		case 16:
		{
			/* 565 */
			red       = (dwBackColor >> 16) & 0xff;
			green     = (dwBackColor >> 8) & 0xff;
			blue      = (dwBackColor >> 0) & 0xff;
			dwBackColor   = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			for (i = 0; i < g_iScreen_Size; i++)
			{
				*pwPen16 = dwBackColor;
				pwPen16++;
				i += 2;
			}
			break;
		}
		case 32:
		{
			for (i = 0; i < g_iScreen_Size; i++)
			{
				*pdwPen32 = dwBackColor;
				pdwPen32++;
				i += 4;
			}
			break;
		}
		default:
		{
			DBG_PRINTF("can't surport %dbpp\n", var.bits_per_pixel);
			return -1;
			break;
		}
	}
}

int FBInit(void)
{
	RegisterDispOpr(&g_tFBDispOpr);
}




