   static void SetUpTestCase() {
    input_ = reinterpret_cast<uint8_t*>(

         vpx_memalign(kDataAlignment, kInputBufferSize + 1)) + 1;
     output_ = reinterpret_cast<uint8_t*>(
         vpx_memalign(kDataAlignment, kOutputBufferSize));
   }
