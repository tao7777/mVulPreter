 virtual void ResetModel() {
    last_pts_ = 0;
    bits_in_buffer_model_ = cfg_.rc_target_bitrate * cfg_.rc_buf_initial_sz;
    frame_number_ = 0;

     first_drop_ = 0;
     bits_total_ = 0;
     duration_ = 0.0;
   }
