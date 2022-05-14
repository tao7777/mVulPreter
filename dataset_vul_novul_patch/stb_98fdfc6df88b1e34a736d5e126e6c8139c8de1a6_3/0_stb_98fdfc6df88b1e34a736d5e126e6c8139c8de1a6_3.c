static int vorbis_finish_frame(stb_vorbis *f, int len, int left, int right)
{
   int prev,i,j;

    if (f->previous_length) {
       int i,j, n = f->previous_length;
       float *w = get_window(f, n);
      if (w == NULL) return 0;
       for (i=0; i < f->channels; ++i) {
          for (j=0; j < n; ++j)
             f->channel_buffers[i][left+j] =
               f->channel_buffers[i][left+j]*w[    j] +
               f->previous_window[i][     j]*w[n-1-j];
      }
   }

   prev = f->previous_length;

   f->previous_length = len - right;

   for (i=0; i < f->channels; ++i)
      for (j=0; right+j < len; ++j)
         f->previous_window[i][j] = f->channel_buffers[i][right+j];

   if (!prev)
      return 0;

   if (len < right) right = len;

   f->samples_output += right-left;

   return right - left;
}
