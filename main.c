#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define xSize 512
#define ySize 512

unsigned char data[3][xSize][ySize] = {0};

unsigned char randdata;

int fputc4LowHigh(unsigned long d, FILE *s)
{
    putc(d & 0xFF, s);
    putc((d >> CHAR_BIT) & 0xFF, s);
    putc((d >> CHAR_BIT * 2) & 0xFF, s);
    return putc((d >> CHAR_BIT * 3) & 0xFF, s);
}

int fputc2LowHigh(unsigned short d, FILE *s)
{
    putc(d & 0xFF, s);
    return putc(d >> CHAR_BIT, s);
}

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

int main()
{

    FILE *fp;
    fp = fopen("icon.bmp", "w");
    putBmpHeader(fp, xSize, ySize, 24);

    for (int i = 0; i < xSize; i++)
    {
        for (int j = 0; j < ySize; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if ((i == 0) || (j == 0))
                {
                    data[k][i][j] = rand() % 255 + 1;
                }
                else
                {
                    data[k][i][j] = (data[k][i][j - 1] + data[k][i][j - 1] + data[k][i][j - 1] + rand() % 255 + 1) / 4;
                }

                fprintf(fp, "%c", data[k][i][j]);
            }
        }
    }
    fclose(fp);
    printf("Program end...");
    return 0;
}