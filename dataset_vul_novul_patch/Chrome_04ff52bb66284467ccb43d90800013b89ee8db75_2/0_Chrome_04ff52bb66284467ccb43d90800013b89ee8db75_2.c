  AudioOutputAuthorizationHandlerTest() {
    base::CommandLine::ForCurrentProcess()->AppendSwitch(
        switches::kUseFakeDeviceForMediaStream);

    thread_bundle_ = base::MakeUnique<TestBrowserThreadBundle>(
        TestBrowserThreadBundle::Options::REAL_IO_THREAD);
    audio_thread_ = base::MakeUnique<AudioManagerThread>();
     audio_manager_.reset(new media::FakeAudioManager(
         audio_thread_->task_runner(), audio_thread_->worker_task_runner(),
         &log_factory_));
    audio_system_ = media::AudioSystemImpl::Create(audio_manager_.get());
     media_stream_manager_ =
         base::MakeUnique<MediaStreamManager>(audio_manager_.get());
    SyncWithAllThreads();
  }
