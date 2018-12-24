#pragma warning(disable : 4996)

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "png.h"
#include "zlib.h"
//将libpng16.dll放置在 exe 文件所在处

/* Makes pngtest verbose so we can find problems. */
#ifndef PNG_DEBUG
#  define PNG_DEBUG 2
#endif

#if PNG_DEBUG > 1
#  define pngtest_debug(m)        ((void)fprintf(stderr, m "\n"))
#  define pngtest_debug1(m,p1)    ((void)fprintf(stderr, m "\n", p1))
#  define pngtest_debug2(m,p1,p2) ((void)fprintf(stderr, m "\n", p1, p2))
#else
#  define pngtest_debug(m)        ((void)0)
#  define pngtest_debug1(m,p1)    ((void)0)
#  define pngtest_debug2(m,p1,p2) ((void)0)
#endif

unsigned char* buffer = NULL;
png_uint_32 width, height;
int color_type;

//获取每一行所用的字节数，需要凑足4的倍数
int getRowBytes(int width) {
	//刚好是4的倍数
	if ((width * 3) % 4 == 0) {
		return width * 3;
	}
	else {
		return ((width * 3) / 4 + 1) * 4;
	}
}

int main(int c, char** v) {

	png_structp png_ptr;
	png_infop info_ptr;
	int bit_depth;
	FILE *fp;

	printf("lpng[%s], zlib[%s]\n", PNG_LIBPNG_VER_STRING, ZLIB_VERSION);

	//打开文件
	if ((fp = fopen("image1.png", "rb")) == NULL) {
		return EXIT_FAILURE;
	}
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		fclose(fp);
		return EXIT_FAILURE;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return EXIT_FAILURE;
	}
	if (setjmp(png_jmpbuf(png_ptr))) {
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		/* If we get here, we had a problem reading the file */
		return EXIT_FAILURE;
	}
	/* Set up the input control if you are using standard C streams */
	png_init_io(png_ptr, fp);
	//读取png文件
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
	//获取png图片相关信息
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
	printf("width[%d], height[%d], bit_depth[%d], color_type[%d]\n",
		width, height, bit_depth, color_type);

	//获得所有png数据
	png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
	//计算buffer大小
	unsigned int bufSize = 0;
	if (color_type == PNG_COLOR_TYPE_RGB) {
		bufSize = getRowBytes(width) * height;
	}
	else if (color_type == PNG_COLOR_TYPE_RGBA) {
		bufSize = width * height * 4;
	}
	else {
		return EXIT_FAILURE;
	}
	//申请堆空间
	buffer = (unsigned char*)malloc(bufSize);
	int i;
	for (i = 0; i < height; i++) {
		//拷贝每行的数据到buffer，
		//opengl原点在下方，拷贝时要倒置一下
		if (color_type == PNG_COLOR_TYPE_RGB) {
			memcpy(buffer + getRowBytes(width) * i, row_pointers[height - i - 1], width * 3);
		}
		else if (color_type == PNG_COLOR_TYPE_RGBA) {
			memcpy(buffer + i * width * 4, row_pointers[height - i - 1], width * 4);
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(fp);

	system("pause");
	return 0;
}
