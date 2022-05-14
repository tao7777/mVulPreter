bool ComponentControllerImpl::BindToRequest(
    fuchsia::sys::Package package,
    fuchsia::sys::StartupInfo startup_info,
    fidl::InterfaceRequest<fuchsia::sys::ComponentController>
        controller_request) {
  DCHECK(!service_directory_);
  DCHECK(!view_provider_binding_);

  url_ = GURL(*package.resolved_url);
  if (!url_.is_valid()) {
    LOG(ERROR) << "Rejected invalid URL: " << url_;
    return false;
  }

  if (controller_request.is_valid()) {
    controller_binding_.Bind(std::move(controller_request));
    controller_binding_.set_error_handler(
         fit::bind_member(this, &ComponentControllerImpl::Kill));
   }
 
  runner_->context()->CreateFrame(frame_observer_binding_.NewBinding(),
                                  frame_.NewRequest());
   frame_->GetNavigationController(navigation_controller_.NewRequest());
   navigation_controller_->LoadUrl(url_.spec(), nullptr);
 
  service_directory_ = std::make_unique<base::fuchsia::ServiceDirectory>(
      std::move(startup_info.launch_info.directory_request));
  view_provider_binding_ = std::make_unique<
      base::fuchsia::ScopedServiceBinding<fuchsia::ui::viewsv1::ViewProvider>>(
      service_directory_.get(), this);

  return true;
}
