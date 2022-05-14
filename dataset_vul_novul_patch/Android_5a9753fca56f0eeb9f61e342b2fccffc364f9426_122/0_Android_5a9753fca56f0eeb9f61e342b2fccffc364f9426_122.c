  LosslessTestLarge()
  LosslessTest()
       : EncoderTest(GET_PARAM(0)),
         psnr_(kMaxPsnr),
         nframes_(0),
         encoding_mode_(GET_PARAM(1)) {
   }
