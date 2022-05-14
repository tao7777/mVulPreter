   static void SetUpTestCase() {
    source_data8_ = reinterpret_cast<uint8_t*>(
         vpx_memalign(kDataAlignment, kDataBlockSize));
    reference_data8_ = reinterpret_cast<uint8_t*>(
         vpx_memalign(kDataAlignment, kDataBufferSize));
    second_pred8_ = reinterpret_cast<uint8_t*>(
        vpx_memalign(kDataAlignment, 64*64));
    source_data16_ = reinterpret_cast<uint16_t*>(
        vpx_memalign(kDataAlignment, kDataBlockSize*sizeof(uint16_t)));
    reference_data16_ = reinterpret_cast<uint16_t*>(
        vpx_memalign(kDataAlignment, kDataBufferSize*sizeof(uint16_t)));
    second_pred16_ = reinterpret_cast<uint16_t*>(
        vpx_memalign(kDataAlignment, 64*64*sizeof(uint16_t)));
   }
