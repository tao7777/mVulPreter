   SvcTest()
       : codec_iface_(0),
         test_file_name_("hantro_collage_w352h288.yuv"),
        stats_file_name_("hantro_collage_w352h288.stat"),
         codec_initialized_(false),
         decoder_(0) {
     memset(&svc_, 0, sizeof(svc_));
    memset(&codec_, 0, sizeof(codec_));
    memset(&codec_enc_, 0, sizeof(codec_enc_));
 }
