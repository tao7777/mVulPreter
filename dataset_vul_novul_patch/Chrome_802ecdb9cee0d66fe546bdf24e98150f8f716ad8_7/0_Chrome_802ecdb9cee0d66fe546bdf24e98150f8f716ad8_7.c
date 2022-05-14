   void TestPlaybackRate(double playback_rate) {
    const int kDefaultBufferSize = algorithm_.samples_per_second() / 10;
    const int kDefaultFramesRequested = 2 * algorithm_.samples_per_second();
 
     TestPlaybackRate(playback_rate, kDefaultBufferSize,
                      kDefaultFramesRequested);
  }
