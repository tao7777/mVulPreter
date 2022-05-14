   void FillRandom(uint8_t *data, int stride) {
     for (int h = 0; h < height_; ++h) {
       for (int w = 0; w < width_; ++w) {
        data[h * stride + w] = rnd_.Rand8();
       }
     }
   }
