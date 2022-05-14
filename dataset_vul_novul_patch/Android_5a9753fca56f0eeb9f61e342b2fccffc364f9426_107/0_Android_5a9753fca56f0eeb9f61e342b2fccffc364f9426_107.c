   virtual void TearDown() {
    ReleaseEncoder();
     delete(decoder_);
  }

  void InitializeEncoder() {
    const vpx_codec_err_t res =
        vpx_svc_init(&svc_, &codec_, vpx_codec_vp9_cx(), &codec_enc_);
    EXPECT_EQ(VPX_CODEC_OK, res);
    vpx_codec_control(&codec_, VP8E_SET_CPUUSED, 4);  // Make the test faster
    vpx_codec_control(&codec_, VP9E_SET_TILE_COLUMNS, tile_columns_);
    vpx_codec_control(&codec_, VP9E_SET_TILE_ROWS, tile_rows_);
    codec_initialized_ = true;
  }

  void ReleaseEncoder() {
    vpx_svc_release(&svc_);
     if (codec_initialized_) vpx_codec_destroy(&codec_);
    codec_initialized_ = false;
  }

  void GetStatsData(std::string *const stats_buf) {
    vpx_codec_iter_t iter = NULL;
    const vpx_codec_cx_pkt_t *cx_pkt;

    while ((cx_pkt = vpx_codec_get_cx_data(&codec_, &iter)) != NULL) {
      if (cx_pkt->kind == VPX_CODEC_STATS_PKT) {
        EXPECT_GT(cx_pkt->data.twopass_stats.sz, 0U);
        ASSERT_TRUE(cx_pkt->data.twopass_stats.buf != NULL);
        stats_buf->append(static_cast<char*>(cx_pkt->data.twopass_stats.buf),
                          cx_pkt->data.twopass_stats.sz);
      }
    }
  }

  void Pass1EncodeNFrames(const int n, const int layers,
                          std::string *const stats_buf) {
    vpx_codec_err_t res;

    ASSERT_GT(n, 0);
    ASSERT_GT(layers, 0);
    svc_.spatial_layers = layers;
    codec_enc_.g_pass = VPX_RC_FIRST_PASS;
    InitializeEncoder();

    libvpx_test::I420VideoSource video(test_file_name_,
                                       codec_enc_.g_w, codec_enc_.g_h,
                                       codec_enc_.g_timebase.den,
                                       codec_enc_.g_timebase.num, 0, 30);
    video.Begin();

    for (int i = 0; i < n; ++i) {
      res = vpx_svc_encode(&svc_, &codec_, video.img(), video.pts(),
                           video.duration(), VPX_DL_GOOD_QUALITY);
      ASSERT_EQ(VPX_CODEC_OK, res);
      GetStatsData(stats_buf);
      video.Next();
    }

    // Flush encoder and test EOS packet.
    res = vpx_svc_encode(&svc_, &codec_, NULL, video.pts(),
                         video.duration(), VPX_DL_GOOD_QUALITY);
    ASSERT_EQ(VPX_CODEC_OK, res);
    GetStatsData(stats_buf);

    ReleaseEncoder();
  }

  void StoreFrames(const size_t max_frame_received,
                   struct vpx_fixed_buf *const outputs,
                   size_t *const frame_received) {
    vpx_codec_iter_t iter = NULL;
    const vpx_codec_cx_pkt_t *cx_pkt;

    while ((cx_pkt = vpx_codec_get_cx_data(&codec_, &iter)) != NULL) {
      if (cx_pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
        const size_t frame_size = cx_pkt->data.frame.sz;

        EXPECT_GT(frame_size, 0U);
        ASSERT_TRUE(cx_pkt->data.frame.buf != NULL);
        ASSERT_LT(*frame_received, max_frame_received);

        if (*frame_received == 0)
          EXPECT_EQ(1, !!(cx_pkt->data.frame.flags & VPX_FRAME_IS_KEY));

        outputs[*frame_received].buf = malloc(frame_size + 16);
        ASSERT_TRUE(outputs[*frame_received].buf != NULL);
        memcpy(outputs[*frame_received].buf, cx_pkt->data.frame.buf,
               frame_size);
        outputs[*frame_received].sz = frame_size;
        ++(*frame_received);
      }
    }
  }

  void Pass2EncodeNFrames(std::string *const stats_buf,
                          const int n, const int layers,
                          struct vpx_fixed_buf *const outputs) {
    vpx_codec_err_t res;
    size_t frame_received = 0;

    ASSERT_TRUE(outputs != NULL);
    ASSERT_GT(n, 0);
    ASSERT_GT(layers, 0);
    svc_.spatial_layers = layers;
    codec_enc_.rc_target_bitrate = 500;
    if (codec_enc_.g_pass == VPX_RC_LAST_PASS) {
      ASSERT_TRUE(stats_buf != NULL);
      ASSERT_GT(stats_buf->size(), 0U);
      codec_enc_.rc_twopass_stats_in.buf = &(*stats_buf)[0];
      codec_enc_.rc_twopass_stats_in.sz = stats_buf->size();
    }
    InitializeEncoder();

    libvpx_test::I420VideoSource video(test_file_name_,
                                       codec_enc_.g_w, codec_enc_.g_h,
                                       codec_enc_.g_timebase.den,
                                       codec_enc_.g_timebase.num, 0, 30);
    video.Begin();

    for (int i = 0; i < n; ++i) {
      res = vpx_svc_encode(&svc_, &codec_, video.img(), video.pts(),
                           video.duration(), VPX_DL_GOOD_QUALITY);
      ASSERT_EQ(VPX_CODEC_OK, res);
      StoreFrames(n, outputs, &frame_received);
      video.Next();
    }

    // Flush encoder.
    res = vpx_svc_encode(&svc_, &codec_, NULL, 0,
                         video.duration(), VPX_DL_GOOD_QUALITY);
    EXPECT_EQ(VPX_CODEC_OK, res);
    StoreFrames(n, outputs, &frame_received);

    EXPECT_EQ(frame_received, static_cast<size_t>(n));

    ReleaseEncoder();
  }

  void DecodeNFrames(const struct vpx_fixed_buf *const inputs, const int n) {
    int decoded_frames = 0;
    int received_frames = 0;

    ASSERT_TRUE(inputs != NULL);
    ASSERT_GT(n, 0);

    for (int i = 0; i < n; ++i) {
      ASSERT_TRUE(inputs[i].buf != NULL);
      ASSERT_GT(inputs[i].sz, 0U);
      const vpx_codec_err_t res_dec =
          decoder_->DecodeFrame(static_cast<const uint8_t *>(inputs[i].buf),
                                inputs[i].sz);
      ASSERT_EQ(VPX_CODEC_OK, res_dec) << decoder_->DecodeError();
      ++decoded_frames;

      DxDataIterator dec_iter = decoder_->GetDxData();
      while (dec_iter.Next() != NULL) {
        ++received_frames;
      }
    }
    EXPECT_EQ(decoded_frames, n);
    EXPECT_EQ(received_frames, n);
  }

  void DropEnhancementLayers(struct vpx_fixed_buf *const inputs,
                             const int num_super_frames,
                             const int remained_spatial_layers) {
    ASSERT_TRUE(inputs != NULL);
    ASSERT_GT(num_super_frames, 0);
    ASSERT_GT(remained_spatial_layers, 0);

    for (int i = 0; i < num_super_frames; ++i) {
      uint32_t frame_sizes[8] = {0};
      int frame_count = 0;
      int frames_found = 0;
      int frame;
      ASSERT_TRUE(inputs[i].buf != NULL);
      ASSERT_GT(inputs[i].sz, 0U);

      vpx_codec_err_t res =
          vp9_parse_superframe_index(static_cast<const uint8_t*>(inputs[i].buf),
                                     inputs[i].sz, frame_sizes, &frame_count,
                                     NULL, NULL);
      ASSERT_EQ(VPX_CODEC_OK, res);

      if (frame_count == 0) {
        // There's no super frame but only a single frame.
        ASSERT_EQ(1, remained_spatial_layers);
      } else {
        // Found a super frame.
        uint8_t *frame_data = static_cast<uint8_t*>(inputs[i].buf);
        uint8_t *frame_start = frame_data;
        for (frame = 0; frame < frame_count; ++frame) {
          // Looking for a visible frame.
          if (frame_data[0] & 0x02) {
            ++frames_found;
            if (frames_found == remained_spatial_layers)
              break;
          }
          frame_data += frame_sizes[frame];
        }
        ASSERT_LT(frame, frame_count) << "Couldn't find a visible frame. "
            << "remained_spatial_layers: " << remained_spatial_layers
            << "    super_frame: " << i;
        if (frame == frame_count - 1)
          continue;

        frame_data += frame_sizes[frame];

        // We need to add one more frame for multiple frame contexts.
        uint8_t marker =
            static_cast<const uint8_t*>(inputs[i].buf)[inputs[i].sz - 1];
        const uint32_t mag = ((marker >> 3) & 0x3) + 1;
        const size_t index_sz = 2 + mag * frame_count;
        const size_t new_index_sz = 2 + mag * (frame + 1);
        marker &= 0x0f8;
        marker |= frame;

        // Copy existing frame sizes.
        memmove(frame_data + 1, frame_start + inputs[i].sz - index_sz + 1,
                new_index_sz - 2);
        // New marker.
        frame_data[0] = marker;
        frame_data += (mag * (frame + 1) + 1);

        *frame_data++ = marker;
        inputs[i].sz = frame_data - frame_start;
      }
    }
  }

  void FreeBitstreamBuffers(struct vpx_fixed_buf *const inputs, const int n) {
    ASSERT_TRUE(inputs != NULL);
    ASSERT_GT(n, 0);

    for (int i = 0; i < n; ++i) {
      free(inputs[i].buf);
      inputs[i].buf = NULL;
      inputs[i].sz = 0;
    }
   }
