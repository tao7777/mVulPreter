  void Initialize(ChannelLayout channel_layout, int bits_per_channel) {
  void Initialize(ChannelLayout channel_layout, int bits_per_channel,
                  int samples_per_second) {
    static const int kFrames = kRawDataSize / ((kDefaultSampleBits / 8) *
        ChannelLayoutToChannelCount(kDefaultChannelLayout));
     AudioParameters params(
         media::AudioParameters::AUDIO_PCM_LINEAR, channel_layout,
        samples_per_second, bits_per_channel, kFrames);
 
     algorithm_.Initialize(1, params, base::Bind(
         &AudioRendererAlgorithmTest::EnqueueData, base::Unretained(this)));
    EnqueueData();
  }
