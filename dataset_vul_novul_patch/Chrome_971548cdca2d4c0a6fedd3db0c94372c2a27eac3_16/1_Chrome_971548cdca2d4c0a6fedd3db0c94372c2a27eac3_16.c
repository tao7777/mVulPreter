 void MediaStreamManager::StopStreamDevice(int render_process_id,
                                           int render_frame_id,
                                           const std::string& device_id,
                                           int session_id) {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
  DVLOG(1) << "StopStreamDevice({render_frame_id = " << render_frame_id << "} "
           << ", {device_id = " << device_id << "}, session_id = " << session_id
           << "})";
