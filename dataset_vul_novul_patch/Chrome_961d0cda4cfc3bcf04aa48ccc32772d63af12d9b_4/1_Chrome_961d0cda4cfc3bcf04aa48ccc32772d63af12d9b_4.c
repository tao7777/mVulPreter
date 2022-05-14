void ChromePasswordManagerClient::AutomaticGenerationStatusChanged(
    bool available,
    const base::Optional<
        autofill::password_generation::PasswordGenerationUIData>& ui_data) {
  if (ui_data &&
      !password_manager::bad_message::CheckChildProcessSecurityPolicy(
          password_generation_driver_bindings_.GetCurrentTargetFrame(),
          ui_data->password_form,
          BadMessageReason::
              CPMD_BAD_ORIGIN_AUTOMATIC_GENERATION_STATUS_CHANGED))
     return;
 #if defined(OS_ANDROID)
  if (PasswordAccessoryController::AllowedForWebContents(web_contents())) {
     if (available) {
       password_manager::PasswordManagerDriver* driver =
           driver_factory_->GetDriverForFrame(
              password_generation_driver_bindings_.GetCurrentTargetFrame());
      DCHECK(driver);
      password_manager_.SetGenerationElementAndReasonForForm(
           driver, ui_data.value().password_form,
           ui_data.value().generation_element,
           false /* is_manually_triggered */);
      PasswordAccessoryController::GetOrCreate(web_contents())
           ->OnAutomaticGenerationStatusChanged(true, ui_data,
                                                driver->AsWeakPtr());
       gfx::RectF element_bounds_in_screen_space = TransformToRootCoordinates(
          password_generation_driver_bindings_.GetCurrentTargetFrame(),
          ui_data.value().bounds);
       driver->GetPasswordAutofillManager()->MaybeShowPasswordSuggestions(
           element_bounds_in_screen_space, ui_data.value().text_direction);
     } else {
      PasswordAccessoryController* accessory =
          PasswordAccessoryController::GetIfExisting(web_contents());
      if (accessory) {
        accessory->OnAutomaticGenerationStatusChanged(false, base::nullopt,
                                                      nullptr);
       }
     }
   }
#else
  if (available) {
    password_manager::ContentPasswordManagerDriver* driver =
        driver_factory_->GetDriverForFrame(
            password_generation_driver_bindings_.GetCurrentTargetFrame());
    ShowPasswordGenerationPopup(driver, ui_data.value(),
                                false /* is_manually_triggered */);
  }
#endif  // defined(OS_ANDROID)
}
