   static void SetUpTestCase() {
    input_ = reinterpret_cast<uint8_t*>(

         vpx_memalign(kDataAlignment, kInputBufferSize + 1)) + 1;
     output_ = reinterpret_cast<uint8_t*>(
         vpx_memalign(kDataAlignment, kOutputBufferSize));
    output_ref_ = reinterpret_cast<uint8_t*>(
        vpx_memalign(kDataAlignment, kOutputBufferSize));
#if CONFIG_VP9_HIGHBITDEPTH
    input16_ = reinterpret_cast<uint16_t*>(
        vpx_memalign(kDataAlignment,
                     (kInputBufferSize + 1) * sizeof(uint16_t))) + 1;
    output16_ = reinterpret_cast<uint16_t*>(
        vpx_memalign(kDataAlignment, (kOutputBufferSize) * sizeof(uint16_t)));
    output16_ref_ = reinterpret_cast<uint16_t*>(
        vpx_memalign(kDataAlignment, (kOutputBufferSize) * sizeof(uint16_t)));
#endif
   }
