void AppControllerImpl::SetClient(mojom::AppControllerClientPtr client) {
   client_ = std::move(client);
 }
