void AppControllerImpl::BindRequest(mojom::AppControllerRequest request) {
void AppControllerService::BindRequest(mojom::AppControllerRequest request) {
   bindings_.AddBinding(this, std::move(request));
 }
