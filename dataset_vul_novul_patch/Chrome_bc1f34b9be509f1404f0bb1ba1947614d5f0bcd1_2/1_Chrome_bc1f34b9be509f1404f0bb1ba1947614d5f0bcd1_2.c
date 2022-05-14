void MediaInterfaceProxy::OnConnectionError() {
   DVLOG(1) << __FUNCTION__;
   DCHECK(thread_checker_.CalledOnValidThread());
 
   interface_factory_ptr_.reset();
 }
