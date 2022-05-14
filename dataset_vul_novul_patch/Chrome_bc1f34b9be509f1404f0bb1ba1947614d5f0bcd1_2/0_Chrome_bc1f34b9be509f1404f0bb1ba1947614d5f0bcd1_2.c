void MediaInterfaceProxy::OnConnectionError() {
media::mojom::InterfaceFactory* MediaInterfaceProxy::GetCdmInterfaceFactory() {
  DVLOG(1) << __FUNCTION__;
  DCHECK(thread_checker_.CalledOnValidThread());
#if !BUILDFLAG(ENABLE_STANDALONE_CDM_SERVICE)
  return GetMediaInterfaceFactory();
#else
  if (!cdm_interface_factory_ptr_)
    ConnectToCdmService();

  DCHECK(cdm_interface_factory_ptr_);

  return cdm_interface_factory_ptr_.get();
#endif
}

void MediaInterfaceProxy::OnMediaServiceConnectionError() {
   DVLOG(1) << __FUNCTION__;
   DCHECK(thread_checker_.CalledOnValidThread());
 
   interface_factory_ptr_.reset();
 }
