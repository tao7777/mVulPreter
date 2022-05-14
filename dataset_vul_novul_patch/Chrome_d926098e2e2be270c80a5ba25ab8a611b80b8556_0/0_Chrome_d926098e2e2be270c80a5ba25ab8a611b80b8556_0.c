 media::interfaces::ServiceFactory* RenderFrameImpl::GetMediaServiceFactory() {
   if (!media_service_factory_) {
    mojo::ServiceProviderPtr service_provider =
        ConnectToApplication(GURL("mojo:media"));
     mojo::ConnectToService(service_provider.get(), &media_service_factory_);
     media_service_factory_.set_connection_error_handler(
         base::Bind(&RenderFrameImpl::OnMediaServiceFactoryConnectionError,
                    base::Unretained(this)));
  }

  return media_service_factory_.get();
}
