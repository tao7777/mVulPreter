 void MediaStreamDispatcherHost::StopStreamDevice(const std::string& device_id,
                                                  int32_t session_id) {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
 
   media_stream_manager_->StopStreamDevice(render_process_id_, render_frame_id_,
                                          device_id, session_id);
 }
