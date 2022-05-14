void EncoderTest::MismatchHook(const vpx_image_t *img1,
void EncoderTest::MismatchHook(const vpx_image_t* /*img1*/,
                               const vpx_image_t* /*img2*/) {
   ASSERT_TRUE(0) << "Encode/Decode mismatch found";
 }
