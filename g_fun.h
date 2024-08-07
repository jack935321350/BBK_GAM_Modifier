#ifndef __G_FUN_H_
#define __G_FUN_H_

int save_buf_bin(const char *fn, void *buf, int len);
void gussianBlur(unsigned char *in, unsigned char *out, int width, int height, int M);
unsigned char calc_otsu(unsigned char *in, int width, int height);
void threshold(unsigned char *in, unsigned char *out, int width, int height, unsigned char thresh);
void adaptive_threshold_S(unsigned char *in, unsigned char *out, int width, int height, int T);

#endif
