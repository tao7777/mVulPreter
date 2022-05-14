void MediaElementAudioSourceHandler::Process(size_t number_of_frames) {
  AudioBus* output_bus = Output(0).Bus();

  MutexTryLocker try_locker(process_lock_);
  if (try_locker.Locked()) {
    if (!MediaElement() || !source_sample_rate_) {
      output_bus->Zero();
      return;
    }

    if (source_number_of_channels_ != output_bus->NumberOfChannels()) {
      output_bus->Zero();
      return;
    }

    AudioSourceProvider& provider = MediaElement()->GetAudioSourceProvider();
    if (multi_channel_resampler_.get()) {
      DCHECK_NE(source_sample_rate_, Context()->sampleRate());
      multi_channel_resampler_->Process(&provider, output_bus,
                                        number_of_frames);
    } else {
      DCHECK_EQ(source_sample_rate_, Context()->sampleRate());
       provider.ProvideInput(output_bus, number_of_frames);
     }
    if (!PassesCORSAccessCheck()) {
      if (maybe_print_cors_message_) {
        maybe_print_cors_message_ = false;
        PostCrossThreadTask(
            *task_runner_, FROM_HERE,
            CrossThreadBind(&MediaElementAudioSourceHandler::PrintCORSMessage,
                            WrapRefCounted(this), current_src_string_));
      }
       output_bus->Zero();
     }
   } else {
    output_bus->Zero();
  }
}
