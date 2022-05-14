void ResourceDispatcherHostImpl::OnReadCompleted(net::URLRequest* request,
                                                 int bytes_read) {
  DCHECK(request);
  VLOG(1) << "OnReadCompleted: \"" << request->url().spec() << "\""
          << " bytes_read = " << bytes_read;
  ResourceRequestInfoImpl* info = ResourceRequestInfoImpl::ForRequest(request);

  if (bytes_read == -1) {
    DCHECK(!request->status().is_success());

    ResponseCompleted(request);
    return;
  }

  info->set_has_started_reading(true);

  if (PauseRequestIfNeeded(info)) {
    info->set_paused_read_bytes(bytes_read);
    VLOG(1) << "OnReadCompleted pausing: \"" << request->url().spec() << "\""
            << " bytes_read = " << bytes_read;
    return;
  }

  if (request->status().is_success() && CompleteRead(request, &bytes_read)) {
    if (info->pause_count() == 0 &&
        Read(request, &bytes_read) &&
        request->status().is_success()) {
      if (bytes_read == 0) {
        CompleteRead(request, &bytes_read);
      } else {
        VLOG(1) << "OnReadCompleted postponing: \""
                << request->url().spec() << "\""
                << " bytes_read = " << bytes_read;
        info->set_paused_read_bytes(bytes_read);
        info->set_is_paused(true);
         GlobalRequestID id(info->GetChildID(), info->GetRequestID());
         MessageLoop::current()->PostTask(
             FROM_HERE,
            base::Bind(&ResourceDispatcherHostImpl::ResumeRequest,
                       AsWeakPtr(), id));
         return;
       }
     }
  }

  if (PauseRequestIfNeeded(info)) {
    info->set_paused_read_bytes(bytes_read);
    VLOG(1) << "OnReadCompleted (CompleteRead) pausing: \""
            << request->url().spec() << "\""
            << " bytes_read = " << bytes_read;
    return;
  }

  if (!request->status().is_io_pending())
    ResponseCompleted(request);
}
