 bool PrintWebViewHelper::UpdatePrintSettings(
     const DictionaryValue& job_settings, bool is_preview) {
  PrintMsg_PrintPages_Params settings;
 
   Send(new PrintHostMsg_UpdatePrintSettings(routing_id(),
       print_pages_params_->params.document_cookie, job_settings, &settings));
   if (settings.params.dpi < kMinDpi || !settings.params.document_cookie) {
     print_preview_context_.set_error(PREVIEW_ERROR_UPDATING_PRINT_SETTINGS);
     return false;
  }

  if (is_preview) {
    if (!job_settings.GetString(printing::kPreviewUIAddr,
                                &(settings.params.preview_ui_addr)) ||
        !job_settings.GetInteger(printing::kPreviewRequestID,
                                 &(settings.params.preview_request_id)) ||
        !job_settings.GetBoolean(printing::kIsFirstRequest,
                                 &(settings.params.is_first_request))) {
      NOTREACHED();
      print_preview_context_.set_error(PREVIEW_ERROR_BAD_SETTING);
      return false;
    }

    if (settings.params.is_first_request &&
        !print_preview_context_.IsModifiable()) {
      settings.params.display_header_footer = false;
    }

    PageSizeMargins default_page_layout;
    GetPageSizeAndMarginsInPoints(NULL, -1, settings.params,
                                  &default_page_layout);
    if (!old_print_pages_params_.get() ||
        !PageLayoutIsEqual(*old_print_pages_params_, settings)) {
      Send(new PrintHostMsg_DidGetDefaultPageLayout(routing_id(),
                                                    default_page_layout));
    }
    SetCustomMarginsIfSelected(job_settings, &settings);

    if (settings.params.display_header_footer) {
      header_footer_info_.reset(new DictionaryValue());
      header_footer_info_->SetString(printing::kSettingHeaderFooterDate,
                                     settings.params.date);
      header_footer_info_->SetString(printing::kSettingHeaderFooterURL,
                                     settings.params.url);
      header_footer_info_->SetString(printing::kSettingHeaderFooterTitle,
                                     settings.params.title);
    }
  }

  print_pages_params_.reset(new PrintMsg_PrintPages_Params(settings));
  Send(new PrintHostMsg_DidGetDocumentCookie(routing_id(),
                                             settings.params.document_cookie));
  return true;
}
