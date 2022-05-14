  unsigned int ReferenceSAD(unsigned int max_sad, int block_idx = 0) {
     unsigned int sad = 0;
    const uint8_t* const reference = GetReference(block_idx);
     for (int h = 0; h < height_; ++h) {
       for (int w = 0; w < width_; ++w) {
        sad += abs(source_data_[h * source_stride_ + w]
               - reference[h * reference_stride_ + w]);
      }
      if (sad > max_sad) {
        break;
       }
     }
     return sad;
   }
