 virtual void ResetModel() {
    last_pts_ = 0;
    bits_in_buffer_model_ = cfg_.rc_target_bitrate * cfg_.rc_buf_initial_sz;
    frame_number_ = 0;

     tot_frame_number_ = 0;
     first_drop_ = 0;
     num_drops_ = 0;
     for (int i = 0; i < 3; ++i) {
       bits_total_[i] = 0;
     }
   }
