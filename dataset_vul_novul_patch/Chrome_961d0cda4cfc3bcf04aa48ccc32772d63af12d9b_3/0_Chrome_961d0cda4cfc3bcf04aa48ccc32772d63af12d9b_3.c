 void ManualFillingControllerImpl::OnGenerationRequested() {
  PasswordGenerationController* pwd_generation_controller =
      pwd_generation_controller_for_testing_
          ? pwd_generation_controller_for_testing_
          : PasswordGenerationController::GetIfExisting(web_contents_);

  DCHECK(pwd_generation_controller);
  pwd_generation_controller->OnGenerationRequested();
 }
