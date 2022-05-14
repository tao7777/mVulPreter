MediaStreamImpl::~MediaStreamImpl() {
  DCHECK(!peer_connection_handler_);
  if (dependency_factory_.get())
    dependency_factory_->ReleasePeerConnectionFactory();
  if (network_manager_) {
    if (chrome_worker_thread_.IsRunning()) {
       chrome_worker_thread_.message_loop()->PostTask(FROM_HERE, base::Bind(
           &MediaStreamImpl::DeleteIpcNetworkManager,
           base::Unretained(this)));
      // Stopping the thread will wait until all tasks have been
      // processed before returning. We wait for the above task to finish before
      // letting the destructor continue to avoid any potential race issues.
      chrome_worker_thread_.Stop();
     } else {
       NOTREACHED() << "Worker thread not running.";
     }
  }
}
