void AutoFillManager::FormSubmitted(const FormData& form) {
  if (!IsAutoFillEnabled())
    return;

   if (tab_contents_->profile()->IsOffTheRecord())
     return;
 
  // Don't save data that was submitted through JavaScript.
  if (!form.user_submitted)
    return;

   upload_form_structure_.reset(new FormStructure(form));
 
  if (!upload_form_structure_->IsAutoFillable())
    return;

  DeterminePossibleFieldTypes(upload_form_structure_.get());
  HandleSubmit();
}
