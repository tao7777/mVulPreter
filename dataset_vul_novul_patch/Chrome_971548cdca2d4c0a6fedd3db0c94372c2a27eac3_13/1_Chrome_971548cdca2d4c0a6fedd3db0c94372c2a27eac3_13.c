 void MediaStreamManager::GenerateStream(
     int render_process_id,
     int render_frame_id,
     int page_request_id,
     const StreamControls& controls,
     MediaDeviceSaltAndOrigin salt_and_origin,
    bool user_gesture,
    GenerateStreamCallback generate_stream_cb,
    DeviceStoppedCallback device_stopped_cb,
    DeviceChangedCallback device_changed_cb) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
   DVLOG(1) << "GenerateStream()";
 
   DeviceRequest* request = new DeviceRequest(
      render_process_id, render_frame_id, page_request_id, user_gesture,
      MEDIA_GENERATE_STREAM, controls, std::move(salt_and_origin),
       std::move(device_stopped_cb));
   request->device_changed_cb = std::move(device_changed_cb);
 
  const std::string& label = AddRequest(request);

  request->generate_stream_cb = std::move(generate_stream_cb);

  if (generate_stream_test_callback_) {
    if (std::move(generate_stream_test_callback_).Run(controls)) {
      FinalizeGenerateStream(label, request);
    } else {
      FinalizeRequestFailed(label, request, MEDIA_DEVICE_INVALID_STATE);
    }
    return;
  }

  base::PostTaskWithTraits(FROM_HERE, {BrowserThread::IO},
                           base::BindOnce(&MediaStreamManager::SetUpRequest,
                                          base::Unretained(this), label));
}
