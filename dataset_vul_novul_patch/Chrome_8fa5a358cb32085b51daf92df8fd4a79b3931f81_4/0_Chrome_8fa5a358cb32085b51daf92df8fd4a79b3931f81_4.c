 void PrintWebViewHelper::OnPrintPreview(const base::DictionaryValue& settings) {
  CHECK_LE(ipc_nesting_level_, 1);

   print_preview_context_.OnPrintPreview();
 
   UMA_HISTOGRAM_ENUMERATION("PrintPreview.PreviewEvent",
                            PREVIEW_EVENT_REQUESTED, PREVIEW_EVENT_MAX);

  if (!print_preview_context_.source_frame()) {
    DidFinishPrinting(FAIL_PREVIEW);
    return;
  }

  if (!UpdatePrintSettings(print_preview_context_.source_frame(),
                           print_preview_context_.source_node(), settings)) {
    if (print_preview_context_.last_error() != PREVIEW_ERROR_BAD_SETTING) {
      Send(new PrintHostMsg_PrintPreviewInvalidPrinterSettings(
          routing_id(), print_pages_params_
                            ? print_pages_params_->params.document_cookie
                            : 0));
      notify_browser_of_print_failure_ = false;  // Already sent.
    }
    DidFinishPrinting(FAIL_PREVIEW);
    return;
  }

  if (print_pages_params_->params.is_first_request &&
      !print_preview_context_.IsModifiable()) {
    PrintHostMsg_SetOptionsFromDocument_Params options;
    if (SetOptionsFromPdfDocument(&options))
      Send(new PrintHostMsg_SetOptionsFromDocument(routing_id(), options));
  }

  is_print_ready_metafile_sent_ = false;

  print_pages_params_->params.supports_alpha_blend = true;

  bool generate_draft_pages = false;
  if (!settings.GetBoolean(kSettingGenerateDraftData, &generate_draft_pages)) {
    NOTREACHED();
  }
  print_preview_context_.set_generate_draft_pages(generate_draft_pages);

  PrepareFrameForPreviewDocument();
}
