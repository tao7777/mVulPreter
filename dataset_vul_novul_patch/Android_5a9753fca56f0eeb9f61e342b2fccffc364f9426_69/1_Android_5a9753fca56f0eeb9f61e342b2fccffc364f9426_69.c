 int GetFreeFrameBuffer(size_t min_size, vpx_codec_frame_buffer_t *fb) {
    EXPECT_TRUE(fb != NULL);
 const int idx = FindFreeBufferIndex();
 if (idx == num_buffers_)
 return -1;


     if (ext_fb_list_[idx].size < min_size) {
       delete [] ext_fb_list_[idx].data;
       ext_fb_list_[idx].data = new uint8_t[min_size];
       ext_fb_list_[idx].size = min_size;
     }
 
 SetFrameBuffer(idx, fb);
 return 0;
 }
