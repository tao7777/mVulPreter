void AppControllerImpl::OnAppUpdate(const apps::AppUpdate& update) {
void AppControllerService::OnAppUpdate(const apps::AppUpdate& update) {
   if (!update.StateIsNull() && !update.NameChanged() &&
       !update.ReadinessChanged()) {
    return;
  }

  if (client_) {
    client_->OnAppChanged(CreateAppPtr(update));
   }
 }
