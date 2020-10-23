/*************************************************************************
 * 作成；H.niwa
 * 参考；http://c-language-program.cocolog-nifty.com/blog/2010/03/bmp-4b2c.html
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define PIXEL_NUM_X (512)                             /* 画像のXサイズ */
#define PIXEL_NUM_Y (512)                             /* 画像のYサイズ */
#define COLOR_BIT (24)                                /* 色ビット */
#define PIC_DATA_SIZE (PIXEL_NUM_X * 3 * PIXEL_NUM_Y) /* bitmapのサイズ */

/* プロトタイプ宣言 */
void createPic(unsigned char *b, int x, int y);
int putBmpHeader(FILE *s, int x, int y, int c);
int fputc2LowHigh(unsigned short d, FILE *s);
int fputc4LowHigh(unsigned long d, FILE *s);

/*
	メイン
*/
int main(int argc, char *argv[])
{
    FILE *f;
    int r;
    unsigned char *b;

    /* bitmap用メモリを確保 */
    b = malloc(PIC_DATA_SIZE);
    if (b == NULL)
    {
        return EXIT_FAILURE;
    }

    /* 画像を生成 */
    createPic(b, PIXEL_NUM_X, PIXEL_NUM_Y);

    /* ファイルをオープン */
    f = fopen("test.bmp", "wb");
    if (f == NULL)
    {
        return EXIT_FAILURE;
    }

    /* ヘッダの書出 */
    r = putBmpHeader(f, PIXEL_NUM_X, PIXEL_NUM_Y, COLOR_BIT);
    if (!r)
    {
        fclose(f);
        return EXIT_FAILURE;
    }

    /* bitmapの書出 */
    r = fwrite(b, sizeof(unsigned char), PIC_DATA_SIZE, f);
    if (r != PIC_DATA_SIZE)
    {
        fclose(f);
        return EXIT_FAILURE;
    }

    /* ファイルをクローズし,処理終了 */
    fclose(f);
    return EXIT_SUCCESS;
}

/* 画像生成関数 */
void createPic(unsigned char *b, int x, int y)
{
    int i;
    int j;

    /* 乱数種設定 */
    srand(time(NULL));

    /* データを生成 */
    for (i = 0; i < y; i++)
    {

        /* 1行分のデータを出力 */
        for (j = 0; j < x; j++)
        {
            *b = rand() % 256;
            b++;
            *b = rand() % 256;
            b++;
            *b = rand() % 256;
            b++;
        }
    }
}

/*
	putBmpHeader BMPヘッダ書出
	
	BMPファイルのヘッダを書き出す

	●戻り値
		int:0…失敗, 1…成功
	●引数
		FILE *s:[i] 出力ストリーム
		int x:[i] 画像Xサイズ(dot, 1〜)
		int y:[i] 画像Yサイズ(dot, 1〜)
		int c:[i] 色ビット数(bit/dot, 1 or 4 or 8 or 24)
*/
int putBmpHeader(FILE *s, int x, int y, int c)
{
    int i;
    int color;                   /* 色数 */
    unsigned long int bfOffBits; /* ヘッダサイズ(byte) */

    /* 画像サイズが異常の場合,エラーでリターン */
    if (x <= 0 || y <= 0)
    {
        return 0;
    }

    /* 出力ストリーム異常の場合,エラーでリターン */
    if (s == NULL || ferror(s))
    {
        return 0;
    }

    /* 色数を計算 */
    if (c == 24)
    {
        color = 0;
    }
    else
    {
        color = 1;
        for (i = 1; i <= c; i++)
        {
            color *= 2;
        }
    }

    /* ヘッダサイズ(byte)を計算 */
    /* ヘッダサイズはビットマップファイルヘッダ(14) + ビットマップ情報ヘッダ(40) + 色数 */
    bfOffBits = 14 + 40 + 4 * color;

    /* ビットマップファイルヘッダ(計14byte)を書出 */
    /* 識別文字列 */
    fputs("BM", s);

    /* bfSize ファイルサイズ(byte) */
    fputc4LowHigh(bfOffBits + (unsigned long)x * y, s);

    /* bfReserved1 予約領域1(byte) */
    fputc2LowHigh(0, s);

    /* bfReserved2 予約領域2(byte) */
    fputc2LowHigh(0, s);

    /* bfOffBits ヘッダサイズ(byte) */
    fputc4LowHigh(bfOffBits, s);

    /* ビットマップ情報ヘッダ(計40byte) */
    /* biSize 情報サイズ(byte) */
    fputc4LowHigh(40, s);

    /* biWidth 画像Xサイズ(dot) */
    fputc4LowHigh(x, s);

    /* biHeight 画像Yサイズ(dot) */
    fputc4LowHigh(y, s);

    /* biPlanes 面数 */
    fputc2LowHigh(1, s);

    /* biBitCount 色ビット数(bit/dot) */
    fputc2LowHigh(c, s);

    /* biCompression 圧縮方式 */
    fputc4LowHigh(0, s);

    /* biSizeImage 圧縮サイズ(byte) */
    fputc4LowHigh(0, s);

    /* biXPelsPerMeter 水平解像度(dot/m) */
    fputc4LowHigh(0, s);

    /* biYPelsPerMeter 垂直解像度(dot/m) */
    fputc4LowHigh(0, s);

    /* biClrUsed 色数 */
    fputc4LowHigh(0, s);

    /* biClrImportant 重要色数 */
    fputc4LowHigh(0, s);

    /* 書出失敗ならエラーでリターン */
    if (ferror(s))
    {
        return 0;
    }

    /* 成功でリターン */
    return 1;
}

/*
	fputc2LowHigh 2バイトデータ書出(下位〜上位)
	
	2バイトのデータを下位〜上位の順で書き出す

	●戻り値
		int:EOF…失敗, EOF以外…成功
	●引数
		unsigned short d:[i] データ
		FILE *s:[i] 出力ストリーム
*/
int fputc2LowHigh(unsigned short d, FILE *s)
{
    putc(d & 0xFF, s);
    return putc(d >> CHAR_BIT, s);
}

/*
	fputc4LowHigh 4バイトデータ書出(下位〜上位)
	
	4バイトのデータを下位〜上位の順で書き出す

	●戻り値
		int:EOF…失敗, EOF以外…成功
	●引数
		unsigned long d:[i] データ
		FILE *s:[i] 出力ストリーム
*/
int fputc4LowHigh(unsigned long d, FILE *s)
{
    putc(d & 0xFF, s);
    putc((d >> CHAR_BIT) & 0xFF, s);
    putc((d >> CHAR_BIT * 2) & 0xFF, s);
    return putc((d >> CHAR_BIT * 3) & 0xFF, s);
}