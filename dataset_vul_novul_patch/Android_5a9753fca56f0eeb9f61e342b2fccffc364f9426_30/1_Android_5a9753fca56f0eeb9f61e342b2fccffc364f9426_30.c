   virtual void SetUp() {
     UUT_ = GET_PARAM(2);
     /* Set up guard blocks for an inner block centered in the outer block */
     for (int i = 0; i < kOutputBufferSize; ++i) {
       if (IsIndexInBorder(i))
        output_[i] = 255;
 else
        output_[i] = 0;

     }
 
     ::libvpx_test::ACMRandom prng;
    for (int i = 0; i < kInputBufferSize; ++i)
      input_[i] = prng.Rand8Extremes();
   }
