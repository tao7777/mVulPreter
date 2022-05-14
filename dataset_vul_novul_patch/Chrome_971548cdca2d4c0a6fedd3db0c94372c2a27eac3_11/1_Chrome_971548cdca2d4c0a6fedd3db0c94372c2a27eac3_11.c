 void MediaStreamManager::CancelRequest(int render_process_id,
                                        int render_frame_id,
                                        int page_request_id) {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
   for (const LabeledDeviceRequest& labeled_request : requests_) {
     DeviceRequest* const request = labeled_request.second;
     if (request->requesting_process_id == render_process_id &&
         request->requesting_frame_id == render_frame_id &&
         request->page_request_id == page_request_id) {
       CancelRequest(labeled_request.first);
       return;
    }
  }
}
