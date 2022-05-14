 void MediaStreamManager::CancelAllRequests(int render_process_id,
                                           int render_frame_id,
                                           int requester_id) {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
   auto request_it = requests_.begin();
   while (request_it != requests_.end()) {
     if (request_it->second->requesting_process_id != render_process_id ||
        request_it->second->requesting_frame_id != render_frame_id ||
        request_it->second->requester_id != requester_id) {
       ++request_it;
       continue;
     }
    const std::string label = request_it->first;
    ++request_it;
    CancelRequest(label);
  }
}
