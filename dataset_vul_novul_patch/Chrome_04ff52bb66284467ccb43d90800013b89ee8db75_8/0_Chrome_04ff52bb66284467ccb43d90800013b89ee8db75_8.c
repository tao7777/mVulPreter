   AudioSystemImplTest()
      : use_audio_thread_(GetParam()),
        audio_thread_("AudioSystemThread"),
        input_params_(AudioParameters::AUDIO_PCM_LINEAR,
                      CHANNEL_LAYOUT_MONO,
                      AudioParameters::kTelephoneSampleRate,
                      16,
                      AudioParameters::kTelephoneSampleRate / 10),
        output_params_(AudioParameters::AUDIO_PCM_LINEAR,
                       CHANNEL_LAYOUT_MONO,
                       AudioParameters::kTelephoneSampleRate,
                       16,
                       AudioParameters::kTelephoneSampleRate / 20),
        default_output_params_(AudioParameters::AUDIO_PCM_LINEAR,
                               CHANNEL_LAYOUT_MONO,
                               AudioParameters::kTelephoneSampleRate,
                               16,
                               AudioParameters::kTelephoneSampleRate / 30) {
     if (use_audio_thread_) {
       audio_thread_.StartAndWaitForTesting();
       audio_manager_.reset(
          new media::MockAudioManager(audio_thread_.task_runner()));
    } else {
       audio_manager_.reset(new media::MockAudioManager(
           base::ThreadTaskRunnerHandle::Get().get()));
     }

    audio_manager_->SetInputStreamParameters(input_params_);
    audio_manager_->SetOutputStreamParameters(output_params_);
    audio_manager_->SetDefaultOutputStreamParameters(default_output_params_);

     audio_system_ = media::AudioSystemImpl::Create(audio_manager_.get());
     EXPECT_EQ(AudioSystem::Get(), audio_system_.get());
   }
