void ResourceDispatcherHost::OnShutdown() {
  DCHECK(ChromeThread::CurrentlyOn(ChromeThread::IO));
  is_shutdown_ = true;
  resource_queue_.Shutdown();
  STLDeleteValues(&pending_requests_);
   update_load_states_timer_.Stop();
 }
