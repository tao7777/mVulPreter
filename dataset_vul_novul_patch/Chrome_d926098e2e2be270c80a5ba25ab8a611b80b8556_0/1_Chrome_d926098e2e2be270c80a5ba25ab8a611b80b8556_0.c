 media::interfaces::ServiceFactory* RenderFrameImpl::GetMediaServiceFactory() {
   if (!media_service_factory_) {
    mojo::InterfacePtr<mojo::Shell> shell_ptr;
    GetServiceRegistry()->ConnectToRemoteService(mojo::GetProxy(&shell_ptr));
    mojo::ServiceProviderPtr service_provider;
    mojo::URLRequestPtr request(mojo::URLRequest::New());
    request->url = mojo::String::From("mojo:media");
    shell_ptr->ConnectToApplication(request.Pass(), GetProxy(&service_provider),
                                    nullptr, nullptr);
     mojo::ConnectToService(service_provider.get(), &media_service_factory_);
     media_service_factory_.set_connection_error_handler(
         base::Bind(&RenderFrameImpl::OnMediaServiceFactoryConnectionError,
                    base::Unretained(this)));
  }

  return media_service_factory_.get();
}
