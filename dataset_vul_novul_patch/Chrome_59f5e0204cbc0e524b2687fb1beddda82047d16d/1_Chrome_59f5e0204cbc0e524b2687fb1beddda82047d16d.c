void AutoFillManager::FormSubmitted(const FormData& form) {
  if (!IsAutoFillEnabled())
    return;

   if (tab_contents_->profile()->IsOffTheRecord())
     return;
 
   upload_form_structure_.reset(new FormStructure(form));
 
  if (!upload_form_structure_->IsAutoFillable())
    return;

  DeterminePossibleFieldTypes(upload_form_structure_.get());
  HandleSubmit();
}
