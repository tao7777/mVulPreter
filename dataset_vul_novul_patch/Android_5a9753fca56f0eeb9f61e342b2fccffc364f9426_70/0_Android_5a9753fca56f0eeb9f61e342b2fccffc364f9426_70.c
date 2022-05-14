   int ReturnFrameBuffer(vpx_codec_frame_buffer_t *fb) {
    if (fb == NULL) {
      EXPECT_TRUE(fb != NULL);
      return -1;
    }
     ExternalFrameBuffer *const ext_fb =
         reinterpret_cast<ExternalFrameBuffer*>(fb->priv);
    if (ext_fb == NULL) {
      EXPECT_TRUE(ext_fb != NULL);
      return -1;
    }
     EXPECT_EQ(1, ext_fb->in_use);
     ext_fb->in_use = 0;
     return 0;
 }
