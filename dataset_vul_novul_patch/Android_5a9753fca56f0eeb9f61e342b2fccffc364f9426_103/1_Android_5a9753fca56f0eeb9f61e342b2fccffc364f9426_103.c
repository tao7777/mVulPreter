   static void SetUpTestCase() {
    source_data_ = reinterpret_cast<uint8_t*>(
         vpx_memalign(kDataAlignment, kDataBlockSize));
    reference_data_ = reinterpret_cast<uint8_t*>(
         vpx_memalign(kDataAlignment, kDataBufferSize));
   }
