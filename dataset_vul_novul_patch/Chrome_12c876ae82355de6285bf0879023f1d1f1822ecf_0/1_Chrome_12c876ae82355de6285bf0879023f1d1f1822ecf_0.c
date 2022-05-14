  MediaStreamManagerTest()
      : thread_bundle_(content::TestBrowserThreadBundle::IO_MAINLOOP) {
    audio_manager_ = std::make_unique<MockAudioManager>();
    audio_system_ =
        std::make_unique<media::AudioSystemImpl>(audio_manager_.get());
    auto video_capture_provider = std::make_unique<MockVideoCaptureProvider>();
    video_capture_provider_ = video_capture_provider.get();
     media_stream_manager_ = std::make_unique<MediaStreamManager>(
         audio_system_.get(), audio_manager_->GetTaskRunner(),
         std::move(video_capture_provider));
     base::RunLoop().RunUntilIdle();
 
     ON_CALL(*video_capture_provider_, DoGetDeviceInfosAsync(_))
        .WillByDefault(Invoke(
            [](VideoCaptureProvider::GetDeviceInfosCallback& result_callback) {
              std::vector<media::VideoCaptureDeviceInfo> stub_results;
              base::ResetAndReturn(&result_callback).Run(stub_results);
            }));
  }
