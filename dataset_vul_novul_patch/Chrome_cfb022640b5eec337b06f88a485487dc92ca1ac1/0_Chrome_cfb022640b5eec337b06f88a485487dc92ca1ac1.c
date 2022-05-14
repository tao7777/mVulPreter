void MediaStreamDispatcherHost::DoOpenDevice(
    int32_t page_request_id,
    const std::string& device_id,
    blink::MediaStreamType type,
    OpenDeviceCallback callback,
    MediaDeviceSaltAndOrigin salt_and_origin) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  if (!MediaStreamManager::IsOriginAllowed(render_process_id_,
                                           salt_and_origin.origin)) {
    std::move(callback).Run(false /* success */, std::string(),
                            blink::MediaStreamDevice());
    return;
   }
 
   media_stream_manager_->OpenDevice(
      render_process_id_, render_frame_id_, requester_id_, page_request_id,
       device_id, type, std::move(salt_and_origin), std::move(callback),
       base::BindRepeating(&MediaStreamDispatcherHost::OnDeviceStopped,
                           weak_factory_.GetWeakPtr()));
}
