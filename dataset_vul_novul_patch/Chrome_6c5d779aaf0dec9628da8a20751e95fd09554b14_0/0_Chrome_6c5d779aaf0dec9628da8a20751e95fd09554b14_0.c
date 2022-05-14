ResourceDispatcherHost::~ResourceDispatcherHost() {
  AsyncResourceHandler::GlobalCleanup();
   STLDeleteValues(&pending_requests_);
 
   user_script_listener_->ShutdownMainThread();
 }
