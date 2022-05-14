  AudioRendererHostTest()
      : log_factory(base::MakeUnique<media::FakeAudioLogFactory>()),
         audio_manager_(base::MakeUnique<FakeAudioManagerWithAssociations>(
             base::ThreadTaskRunnerHandle::Get(),
             log_factory.get())),
         render_process_host_(&browser_context_, &auth_run_loop_) {
     base::CommandLine::ForCurrentProcess()->AppendSwitch(
         switches::kUseFakeDeviceForMediaStream);
     media_stream_manager_.reset(new MediaStreamManager(audio_manager_.get()));
     host_ = new MockAudioRendererHost(
         &auth_run_loop_, render_process_host_.GetID(), audio_manager_.get(),
        &mirroring_manager_, media_stream_manager_.get(), kSalt);
 
     host_->set_peer_process_for_testing(base::Process::Current());
  }
