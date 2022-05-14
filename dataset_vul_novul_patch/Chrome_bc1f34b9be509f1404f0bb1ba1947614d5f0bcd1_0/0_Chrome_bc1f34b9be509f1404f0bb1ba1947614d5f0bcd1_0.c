 void MediaInterfaceProxy::CreateCdm(
     media::mojom::ContentDecryptionModuleRequest request) {
   DCHECK(thread_checker_.CalledOnValidThread());
  GetCdmInterfaceFactory()->CreateCdm(std::move(request));
 }
