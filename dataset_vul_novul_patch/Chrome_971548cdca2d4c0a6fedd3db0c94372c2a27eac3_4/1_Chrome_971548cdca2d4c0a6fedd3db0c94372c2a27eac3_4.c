void MediaStreamDispatcherHost::DoGenerateStream(
    int32_t page_request_id,
    const StreamControls& controls,
    bool user_gesture,
    GenerateStreamCallback callback,
    MediaDeviceSaltAndOrigin salt_and_origin) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  if (!MediaStreamManager::IsOriginAllowed(render_process_id_,
                                           salt_and_origin.origin)) {
    std::move(callback).Run(MEDIA_DEVICE_INVALID_SECURITY_ORIGIN, std::string(),
                            MediaStreamDevices(), MediaStreamDevices());
    return;
   }
 
   media_stream_manager_->GenerateStream(
      render_process_id_, render_frame_id_, page_request_id, controls,
      std::move(salt_and_origin), user_gesture, std::move(callback),
       base::BindRepeating(&MediaStreamDispatcherHost::OnDeviceStopped,
                           weak_factory_.GetWeakPtr()),
       base::BindRepeating(&MediaStreamDispatcherHost::OnDeviceChanged,
                          weak_factory_.GetWeakPtr()));
}
