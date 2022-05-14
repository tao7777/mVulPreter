static float *get_window(vorb *f, int len)
{
    len <<= 1;
    if (len == f->blocksize_0) return f->window[0];
    if (len == f->blocksize_1) return f->window[1];
    return NULL;
 }
