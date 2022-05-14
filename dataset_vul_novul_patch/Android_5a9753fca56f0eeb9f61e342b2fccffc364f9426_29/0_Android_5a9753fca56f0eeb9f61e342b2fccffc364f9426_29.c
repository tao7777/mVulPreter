   void SetConstantInput(int value) {
     memset(input_, value, kInputBufferSize);
#if CONFIG_VP9_HIGHBITDEPTH
    vpx_memset16(input16_, value, kInputBufferSize);
#endif
  }

  void CopyOutputToRef() {
    memcpy(output_ref_, output_, kOutputBufferSize);
#if CONFIG_VP9_HIGHBITDEPTH
    memcpy(output16_ref_, output16_, kOutputBufferSize);
#endif
   }
