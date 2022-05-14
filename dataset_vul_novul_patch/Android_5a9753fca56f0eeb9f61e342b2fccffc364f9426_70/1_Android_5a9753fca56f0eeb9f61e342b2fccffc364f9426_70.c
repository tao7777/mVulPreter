   int ReturnFrameBuffer(vpx_codec_frame_buffer_t *fb) {
    EXPECT_TRUE(fb != NULL);
     ExternalFrameBuffer *const ext_fb =
         reinterpret_cast<ExternalFrameBuffer*>(fb->priv);
    EXPECT_TRUE(ext_fb != NULL);
     EXPECT_EQ(1, ext_fb->in_use);
     ext_fb->in_use = 0;
     return 0;
 }
