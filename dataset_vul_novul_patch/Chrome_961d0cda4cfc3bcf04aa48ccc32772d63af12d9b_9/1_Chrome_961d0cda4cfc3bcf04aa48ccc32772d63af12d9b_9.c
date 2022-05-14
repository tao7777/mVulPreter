void PasswordAccessoryControllerImpl::OnAutomaticGenerationStatusChanged(
    bool available,
    const base::Optional<
        autofill::password_generation::PasswordGenerationUIData>& ui_data,
    const base::WeakPtr<password_manager::PasswordManagerDriver>& driver) {
  target_frame_driver_ = driver;
  if (available) {
    DCHECK(ui_data.has_value());
    generation_element_data_ = std::make_unique<GenerationElementData>(
        ui_data.value().password_form,
        autofill::CalculateFormSignature(
            ui_data.value().password_form.form_data),
        autofill::CalculateFieldSignatureByNameAndType(
            ui_data.value().generation_element, "password"),
        ui_data.value().max_length);
  } else {
    generation_element_data_.reset();
  }
  GetManualFillingController()->OnAutomaticGenerationStatusChanged(available);
}
