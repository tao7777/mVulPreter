  void TestPlaybackRate(double playback_rate,
                        int buffer_size_in_frames,
                        int total_frames_requested) {
    int initial_bytes_enqueued = bytes_enqueued_;
    int initial_bytes_buffered = algorithm_.bytes_buffered();

    algorithm_.SetPlaybackRate(static_cast<float>(playback_rate));

    scoped_array<uint8> buffer(
        new uint8[buffer_size_in_frames * algorithm_.bytes_per_frame()]);

    if (playback_rate == 0.0) {
      int frames_written =
          algorithm_.FillBuffer(buffer.get(), buffer_size_in_frames);
      EXPECT_EQ(0, frames_written);
      return;
    }

    int frames_remaining = total_frames_requested;
    while (frames_remaining > 0) {
       int frames_requested = std::min(buffer_size_in_frames, frames_remaining);
       int frames_written =
           algorithm_.FillBuffer(buffer.get(), frames_requested);
      CHECK_GT(frames_written, 0);
      CheckFakeData(buffer.get(), frames_written, playback_rate);
       frames_remaining -= frames_written;
     }
 
    int bytes_requested = total_frames_requested * algorithm_.bytes_per_frame();
    int bytes_consumed = ComputeConsumedBytes(initial_bytes_enqueued,
                                              initial_bytes_buffered);

    if (playback_rate == 1.0) {
      EXPECT_EQ(bytes_requested, bytes_consumed);
      return;
    }

    static const double kMaxAcceptableDelta = 0.01;
    double actual_playback_rate = 1.0 * bytes_consumed / bytes_requested;

    double delta = std::abs(1.0 - (actual_playback_rate / playback_rate));

    EXPECT_LE(delta, kMaxAcceptableDelta);
  }
