 void PrintWebViewHelper::OnPrintPreview(const DictionaryValue& settings) {
   DCHECK(is_preview_enabled_);
   print_preview_context_.OnPrintPreview();

  if (!UpdatePrintSettings(print_preview_context_.frame(),
                           print_preview_context_.node(), settings)) {
    if (print_preview_context_.last_error() != PREVIEW_ERROR_BAD_SETTING) {
      Send(new PrintHostMsg_PrintPreviewInvalidPrinterSettings(
          routing_id(), print_pages_params_->params.document_cookie));
      notify_browser_of_print_failure_ = false;  // Already sent.
    }
    DidFinishPrinting(FAIL_PREVIEW);
    return;
  }

  if (!print_pages_params_->params.is_first_request &&
      old_print_pages_params_.get() &&
      PrintMsg_Print_Params_IsEqual(*old_print_pages_params_,
                                    *print_pages_params_)) {
    PrintHostMsg_DidPreviewDocument_Params preview_params;
    preview_params.reuse_existing_data = true;
    preview_params.data_size = 0;
    preview_params.document_cookie =
        print_pages_params_->params.document_cookie;
    preview_params.expected_pages_count =
        print_preview_context_.total_page_count();
    preview_params.modifiable = print_preview_context_.IsModifiable();
    preview_params.preview_request_id =
        print_pages_params_->params.preview_request_id;

    Send(new PrintHostMsg_MetafileReadyForPrinting(routing_id(),
                                                    preview_params));
     return;
   }
   old_print_pages_params_.reset();
   is_print_ready_metafile_sent_ = false;

  print_pages_params_->params.supports_alpha_blend = true;

  bool generate_draft_pages = false;
  if (!settings.GetBoolean(printing::kSettingGenerateDraftData,
                           &generate_draft_pages)) {
    NOTREACHED();
  }
  print_preview_context_.set_generate_draft_pages(generate_draft_pages);

  if (CreatePreviewDocument()) {
    DidFinishPrinting(OK);
  } else {
    if (notify_browser_of_print_failure_)
      LOG(ERROR) << "CreatePreviewDocument failed";
    DidFinishPrinting(FAIL_PREVIEW);
  }
}
