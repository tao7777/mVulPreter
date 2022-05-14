   AudioSystemImplTest()
      : use_audio_thread_(GetParam()), audio_thread_("AudioSystemThread") {
     if (use_audio_thread_) {
       audio_thread_.StartAndWaitForTesting();
       audio_manager_.reset(
          new media::MockAudioManager(audio_thread_.task_runner()));
    } else {
       audio_manager_.reset(new media::MockAudioManager(
           base::ThreadTaskRunnerHandle::Get().get()));
     }
    audio_manager_->SetInputStreamParameters(
        media::AudioParameters::UnavailableDeviceParams());
     audio_system_ = media::AudioSystemImpl::Create(audio_manager_.get());
     EXPECT_EQ(AudioSystem::Get(), audio_system_.get());
   }
