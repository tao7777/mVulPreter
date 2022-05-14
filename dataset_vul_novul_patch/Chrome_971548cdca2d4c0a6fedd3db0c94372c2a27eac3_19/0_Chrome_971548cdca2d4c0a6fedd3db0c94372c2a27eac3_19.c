   void OpenSession() {
     const int render_process_id = 1;
     const int render_frame_id = 1;
    const int requester_id = 1;
     const int page_request_id = 1;
     const url::Origin security_origin =
         url::Origin::Create(GURL("http://test.com"));

    ASSERT_TRUE(opened_device_label_.empty());

    MediaDeviceInfoArray video_devices;
    {
      base::RunLoop run_loop;
      MediaDevicesManager::BoolDeviceTypes devices_to_enumerate;
      devices_to_enumerate[MEDIA_DEVICE_TYPE_VIDEO_INPUT] = true;
      media_stream_manager_->media_devices_manager()->EnumerateDevices(
          devices_to_enumerate,
          base::BindOnce(&VideoInputDevicesEnumerated, run_loop.QuitClosure(),
                         browser_context_.GetMediaDeviceIDSalt(),
                         security_origin, &video_devices));
      run_loop.Run();
    }
    ASSERT_FALSE(video_devices.empty());

     {
       base::RunLoop run_loop;
       media_stream_manager_->OpenDevice(
          render_process_id, render_frame_id, requester_id, page_request_id,
           video_devices[0].device_id, MEDIA_DEVICE_VIDEO_CAPTURE,
           MediaDeviceSaltAndOrigin{browser_context_.GetMediaDeviceIDSalt(),
                                    browser_context_.GetMediaDeviceIDSalt(),
                                   security_origin},
          base::BindOnce(&VideoCaptureTest::OnDeviceOpened,
                         base::Unretained(this), run_loop.QuitClosure()),
          MediaStreamManager::DeviceStoppedCallback());
      run_loop.Run();
    }
    ASSERT_NE(MediaStreamDevice::kNoId, opened_session_id_);
  }
