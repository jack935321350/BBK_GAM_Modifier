#include "g_fun.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int save_buf_bin(const char *fn, void *buf, int len)
{
    FILE *fp = NULL;
    errno_t err = fopen_s(&fp, fn, "wb");
    if(err != 0 || fp == NULL) return -1;
    fwrite(buf, 1, len, fp);
    fflush(fp);
    fclose(fp);
    return 0;
}

static double *gussianFilter(int M)
{
    if(M % 2 == 0) return NULL;
    double sum = 0.;
    double sigma = 1.;
    double *gussian  = (double *)malloc(M * M * sizeof(double));
    if(gussian == NULL) return gussian;
    for(int y = 0; y < M; y++)
    {
        for(int x = 0; x < M; x++)
        {
            gussian[y * M + x] = exp(-((y - M / 2) * (y - M / 2) + (x - M / 2) * (x - M / 2)) / (2.0 * sigma * sigma));
            sum += gussian[y * M + x];
        }
    }
    for(int y = 0; y < M; y++)
    {
        for(int x = 0; x < M; x++)
        {
            gussian[y * M + x] = gussian[y * M + x] / sum;
        }
    }
    return gussian;
}

void gussianBlur(unsigned char *in, unsigned char *out, int width, int height, int M)
{
    if(M % 2 == 0) return;
    int mask = M / 2;
    memcpy(out, in, width * height);
    double *filter = gussianFilter(M);
    if(filter == NULL) return;
    for (int y = mask; y < height - mask; y++)
    {
        for (int x = mask; x < width - mask; x++)
        {
            double sum = 0;
            int cnt = 0;
            for(int my = -mask; my <= mask; my++)
            {
                for(int mx = -mask; mx <= mask; mx++)
                {
                    sum += (in[(y + my) * width + x + mx] * filter[cnt]);
                    cnt++;
                }
            }
            out[y * width + x] = (unsigned char)sum;
        }
    }
    free(filter);
}

unsigned char calc_otsu(unsigned char *in, int width, int height)
{
    int pixelCount[256] = {0};
    float pixelPercent[256] = {0};


    unsigned char threshold = 0;

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            pixelCount[in[y * width + x]]++;
        }
    }

    int totalPixelCount = width * height;
    float u = 0;
    for(int i = 0; i < 256; i++)
    {
        pixelPercent[i] = (float)pixelCount[i] / totalPixelCount;
        u += i * pixelPercent[i];       //总平均灰度
    }

    float maxVariance = 0.0;            //最大类间方差
    float w0 = 0, avgValue  = 0;        //w0 前景比例 ，avgValue 前景平均灰度
    for(int i = 0; i < 256; i++)        //每一次循环都是一次完整类间方差计算 (两个for叠加为1个)
    {
        w0 += pixelPercent[i];          //假设当前灰度i为阈值, 0 ~ i 灰度像素所占整幅图像的比例即前景比例
        avgValue  += i * pixelPercent[i];

        float variance = (float)pow((avgValue / w0 - u), 2) * w0 /(1 - w0);    //类间方差
        if(variance > maxVariance)
        {
            maxVariance = variance;
            threshold = i;
        }
    }

    return threshold;

}

void threshold(unsigned char *in, unsigned char *out, int width, int height, unsigned char thresh)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (in[y * width + x] <= thresh)
            {
                out[y * width + x] = 255;
            }
            else
            {
                out[y * width + x] = 0;
            }
        }
    }
}

void adaptive_threshold_S(unsigned char *in, unsigned char *out, int width, int height, int T)
{
    int s2 = width >> 4; // S = Width / 8, so S / 2 = Width / 16

    unsigned int *integral = (unsigned int *)malloc(width * height * sizeof(unsigned int));
    if(integral == NULL) return;
    memset(integral, 0, width * height * sizeof(unsigned int));

    for (int x = 0; x < width; x++)
    {
        unsigned int sum = 0;
        for (int y = 0; y < height; y++)
        {
            sum += in[y * width + x];
            if(x == 0) integral[y * width + x] = sum;
            else integral[y * width + x] = integral[y * width + x - 1] + sum;
        }
    }

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            int index = y * width + x;
            int x1 = x - s2;
            if(x1 < 0) x1 = 0;
            int y1 = y - s2;
            if(y1 < 0) y1 = 0;
            int x2 = x + s2;
            if(x2 >= width) x2 = width - 1;
            int y2 = y + s2;
            if(y2 >= height) y2 = height - 1;
            int count = (x2 - x1) * (y2 - y1);
            // I(x, y) = s(x2, y2) - s(x1, y2) - s(x2, y1) + s(x1, x1)
            unsigned int sum = integral[y2 * width + x2]  - integral[y2 * width + x1] - integral[y1 * width + x2] + integral[y1 * width + x1];
            if(((unsigned int)in[index] * count) < (sum * (100 - T) / 100)) out[index] = 255;
            else out[index] = 0;
        }
    }

    free(integral);
}
