 void MediaStreamDispatcherHost::CancelAllRequests() {
  if (!bindings_.empty())
    return;
  media_stream_manager_->CancelAllRequests(render_process_id_,
                                           render_frame_id_);
 }
