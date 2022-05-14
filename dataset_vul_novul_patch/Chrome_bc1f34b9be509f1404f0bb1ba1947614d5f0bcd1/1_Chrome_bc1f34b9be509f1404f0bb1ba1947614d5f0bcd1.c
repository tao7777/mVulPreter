void MediaInterfaceProxy::ConnectToService() {
   DVLOG(1) << __FUNCTION__;
   DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(!interface_factory_ptr_);
 
   service_manager::mojom::InterfaceProviderPtr interfaces;
   auto provider = base::MakeUnique<media::MediaInterfaceProvider>(
       mojo::MakeRequest(&interfaces));
 #if BUILDFLAG(ENABLE_MOJO_CDM)
   net::URLRequestContextGetter* context_getter =
      BrowserContext::GetDefaultStoragePartition(
          render_frame_host_->GetProcess()->GetBrowserContext())
          ->GetURLRequestContext();
   provider->registry()->AddInterface(base::Bind(
       &ProvisionFetcherImpl::Create, base::RetainedRef(context_getter)));
 #endif  // BUILDFLAG(ENABLE_MOJO_CDM)
   GetContentClient()->browser()->ExposeInterfacesToMediaService(
       provider->registry(), render_frame_host_);
 
   media_registries_.push_back(std::move(provider));
 
   media::mojom::MediaServicePtr media_service;
   service_manager::Connector* connector =
       ServiceManagerConnection::GetForProcess()->GetConnector();
   connector->BindInterface(media::mojom::kMediaServiceName, &media_service);
   media_service->CreateInterfaceFactory(MakeRequest(&interface_factory_ptr_),
                                        std::move(interfaces));
  interface_factory_ptr_.set_connection_error_handler(base::Bind(
      &MediaInterfaceProxy::OnConnectionError, base::Unretained(this)));
 }
