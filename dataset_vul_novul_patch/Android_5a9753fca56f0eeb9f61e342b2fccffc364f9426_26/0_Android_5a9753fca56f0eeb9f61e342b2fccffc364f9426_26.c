 virtual void PreEncodeFrameHook(::libvpx_test::VideoSource *video,
 ::libvpx_test::Encoder *encoder) {

     if (video->frame() == 1) {
       encoder->Control(VP8E_SET_CPUUSED, cpu_used_);
     } else if (video->frame() == 3) {
      vpx_active_map_t map = vpx_active_map_t();
       uint8_t active_map[9 * 13] = {
         1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0,
         1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0,
 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0,
 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0,
 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1,
 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1,
 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1,
 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0,
 };
 map.cols = (kWidth + 15) / 16;
 map.rows = (kHeight + 15) / 16;
      ASSERT_EQ(map.cols, 13u);
      ASSERT_EQ(map.rows, 9u);

       map.active_map = active_map;
       encoder->Control(VP8E_SET_ACTIVEMAP, &map);
     } else if (video->frame() == 15) {
      vpx_active_map_t map = vpx_active_map_t();
       map.cols = (kWidth + 15) / 16;
       map.rows = (kHeight + 15) / 16;
       map.active_map = NULL;
      encoder->Control(VP8E_SET_ACTIVEMAP, &map);
 }
 }
