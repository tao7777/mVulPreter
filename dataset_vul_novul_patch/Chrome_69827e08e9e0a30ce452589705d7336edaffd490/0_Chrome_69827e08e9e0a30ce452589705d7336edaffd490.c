void MediaStreamDevicesController::Accept(bool update_content_setting) {
  if (content_settings_)
    content_settings_->OnMediaStreamAllowed();

  NotifyUIRequestAccepted();

  content::MediaStreamDevices devices;
  if (microphone_requested_ || webcam_requested_) {
    switch (request_.request_type) {
      case content::MEDIA_OPEN_DEVICE:
        MediaCaptureDevicesDispatcher::GetInstance()->GetRequestedDevice(
            request_.requested_device_id,
            request_.audio_type == content::MEDIA_DEVICE_AUDIO_CAPTURE,
            request_.video_type == content::MEDIA_DEVICE_VIDEO_CAPTURE,
            &devices);
        break;
      case content::MEDIA_DEVICE_ACCESS:
      case content::MEDIA_GENERATE_STREAM:
      case content::MEDIA_ENUMERATE_DEVICES:
        MediaCaptureDevicesDispatcher::GetInstance()->
            GetDefaultDevicesForProfile(profile_,
                                        microphone_requested_,
                                        webcam_requested_,
                                        &devices);
         break;
     }
 
    // TODO(raymes): We currently set the content permission for non-https
    // websites for Pepper requests as well. This is temporary and should be
    // removed.
    if (update_content_setting) {
      if ((IsSchemeSecure() && !devices.empty()) ||
          request_.request_type == content::MEDIA_OPEN_DEVICE) {
        SetPermission(true);
      }
    }
   }
 
   scoped_ptr<content::MediaStreamUI> ui;
  if (!devices.empty()) {
    ui = MediaCaptureDevicesDispatcher::GetInstance()->
        GetMediaStreamCaptureIndicator()->RegisterMediaStream(
            web_contents_, devices);
  }
  content::MediaResponseCallback cb = callback_;
  callback_.Reset();
  cb.Run(devices, ui.Pass());
}
