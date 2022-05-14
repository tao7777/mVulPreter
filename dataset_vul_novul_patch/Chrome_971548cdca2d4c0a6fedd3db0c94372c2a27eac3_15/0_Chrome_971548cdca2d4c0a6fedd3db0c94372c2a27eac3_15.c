 void MediaStreamManager::OpenDevice(int render_process_id,
                                     int render_frame_id,
                                    int requester_id,
                                     int page_request_id,
                                     const std::string& device_id,
                                     MediaStreamType type,
                                    MediaDeviceSaltAndOrigin salt_and_origin,
                                    OpenDeviceCallback open_device_cb,
                                    DeviceStoppedCallback device_stopped_cb) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  DCHECK(type == MEDIA_DEVICE_AUDIO_CAPTURE ||
         type == MEDIA_DEVICE_VIDEO_CAPTURE);
  DVLOG(1) << "OpenDevice ({page_request_id = " << page_request_id << "})";
  StreamControls controls;
  if (IsAudioInputMediaType(type)) {
    controls.audio.requested = true;
    controls.audio.stream_type = type;
    controls.audio.device_id = device_id;
  } else if (IsVideoInputMediaType(type)) {
    controls.video.requested = true;
    controls.video.stream_type = type;
    controls.video.device_id = device_id;
  } else {
     NOTREACHED();
   }
   DeviceRequest* request = new DeviceRequest(
      render_process_id, render_frame_id, requester_id, page_request_id,
       false /* user gesture */, MEDIA_OPEN_DEVICE_PEPPER_ONLY, controls,
       std::move(salt_and_origin), std::move(device_stopped_cb));
 
  const std::string& label = AddRequest(request);

  request->open_device_cb = std::move(open_device_cb);
  base::PostTaskWithTraits(FROM_HERE, {BrowserThread::IO},
                           base::BindOnce(&MediaStreamManager::SetUpRequest,
                                          base::Unretained(this), label));
}
