 void MediaStreamDispatcherHost::CancelAllRequests() {
  media_stream_manager_->CancelAllRequests(render_process_id_, render_frame_id_,
                                           requester_id_);
 }
