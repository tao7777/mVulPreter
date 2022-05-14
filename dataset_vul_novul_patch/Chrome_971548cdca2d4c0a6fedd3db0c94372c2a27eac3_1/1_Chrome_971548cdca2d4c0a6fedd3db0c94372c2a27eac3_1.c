void MediaStreamDispatcherHost::BindRequest(
     mojom::MediaStreamDispatcherHostRequest request) {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
  bindings_.AddBinding(this, std::move(request));
 }
