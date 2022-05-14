void ChildThread::Shutdown() {
   file_system_dispatcher_.reset();
   quota_dispatcher_.reset();
  WebFileSystemImpl::DeleteThreadSpecificInstance();
 }
