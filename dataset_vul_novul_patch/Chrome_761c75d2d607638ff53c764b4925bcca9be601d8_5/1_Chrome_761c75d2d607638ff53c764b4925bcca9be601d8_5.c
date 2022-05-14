 void MediaElementAudioSourceHandler::SetFormat(size_t number_of_channels,
                                                float source_sample_rate) {
   if (number_of_channels != source_number_of_channels_ ||
       source_sample_rate != source_sample_rate_) {
     if (!number_of_channels ||
        number_of_channels > BaseAudioContext::MaxNumberOfChannels() ||
        !AudioUtilities::IsValidAudioBufferSampleRate(source_sample_rate)) {
      DLOG(ERROR) << "setFormat(" << number_of_channels << ", "
                  << source_sample_rate << ") - unhandled format change";
       Locker<MediaElementAudioSourceHandler> locker(*this);
       source_number_of_channels_ = 0;
       source_sample_rate_ = 0;
       return;
     }
 
     Locker<MediaElementAudioSourceHandler> locker(*this);
 
     source_number_of_channels_ = number_of_channels;
     source_sample_rate_ = source_sample_rate;
 
    if (source_sample_rate != Context()->sampleRate()) {
      double scale_factor = source_sample_rate / Context()->sampleRate();
      multi_channel_resampler_ = std::make_unique<MultiChannelResampler>(
          scale_factor, number_of_channels);
    } else {
      multi_channel_resampler_.reset();
    }

    {
      BaseAudioContext::GraphAutoLocker context_locker(Context());

      Output(0).SetNumberOfChannels(number_of_channels);
    }
   }
 }
