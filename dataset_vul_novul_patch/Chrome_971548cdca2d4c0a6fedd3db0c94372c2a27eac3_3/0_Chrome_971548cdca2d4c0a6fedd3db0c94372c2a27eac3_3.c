void MediaStreamDispatcherHost::CancelRequest(int page_request_id) {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
 
   media_stream_manager_->CancelRequest(render_process_id_, render_frame_id_,
                                       requester_id_, page_request_id);
 }
