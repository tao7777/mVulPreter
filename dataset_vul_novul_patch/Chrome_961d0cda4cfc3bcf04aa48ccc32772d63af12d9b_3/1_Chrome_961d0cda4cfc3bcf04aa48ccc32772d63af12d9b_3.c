 void ManualFillingControllerImpl::OnGenerationRequested() {
  DCHECK(pwd_controller_);
  pwd_controller_->OnGenerationRequested();
 }
