void AppControllerImpl::SetClient(mojom::AppControllerClientPtr client) {
void AppControllerService::SetClient(mojom::AppControllerClientPtr client) {
   client_ = std::move(client);
 }
