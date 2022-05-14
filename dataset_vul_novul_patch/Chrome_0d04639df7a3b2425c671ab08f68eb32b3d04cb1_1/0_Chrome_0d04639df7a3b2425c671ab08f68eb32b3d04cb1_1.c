void AutoFillManager::HandleSubmit() {
  std::vector<FormStructure*> import;
  import.push_back(upload_form_structure_.get());
  if (!personal_data_->ImportFormData(import, this))
    return;

  AutoFillProfile* profile;
   CreditCard* credit_card;
   personal_data_->GetImportedFormData(&profile, &credit_card);
 
  if (!credit_card) {
     UploadFormData();
    return;
  }

  // Show an infobar to offer to save the credit card info.
  if (tab_contents_) {
    tab_contents_->AddInfoBar(new AutoFillCCInfoBarDelegate(tab_contents_,
                                                            this));
   }
 }
