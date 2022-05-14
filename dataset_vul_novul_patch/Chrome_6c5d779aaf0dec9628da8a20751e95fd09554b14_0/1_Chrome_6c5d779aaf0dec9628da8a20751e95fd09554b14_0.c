ResourceDispatcherHost::~ResourceDispatcherHost() {
  AsyncResourceHandler::GlobalCleanup();
   STLDeleteValues(&pending_requests_);
 
   user_script_listener_->ShutdownMainThread();
  std::set<ProcessRouteIDs> ids;
  for (BlockedRequestMap::const_iterator iter = blocked_requests_map_.begin();
       iter != blocked_requests_map_.end(); ++iter) {
    std::pair<std::set<ProcessRouteIDs>::iterator, bool> result =
        ids.insert(iter->first);
    DCHECK(result.second);
  }
  for (std::set<ProcessRouteIDs>::const_iterator iter = ids.begin();
       iter != ids.end(); ++iter) {
    CancelBlockedRequestsForRoute(iter->first, iter->second);
  }
 }
