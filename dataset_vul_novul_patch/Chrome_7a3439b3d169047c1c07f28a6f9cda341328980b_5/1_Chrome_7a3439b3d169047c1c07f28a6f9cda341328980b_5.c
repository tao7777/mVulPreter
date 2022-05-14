bool PrintWebViewHelper::UpdatePrintSettings(
    WebKit::WebFrame* frame, const WebKit::WebNode& node,
    const DictionaryValue& passed_job_settings) {
  DCHECK(is_preview_enabled_);
  const DictionaryValue* job_settings = &passed_job_settings;
  DictionaryValue modified_job_settings;
  if (job_settings->empty()) {
    if (!print_for_preview_)
      print_preview_context_.set_error(PREVIEW_ERROR_BAD_SETTING);
    return false;
  }

  bool source_is_html = true;
  if (print_for_preview_) {
    if (!job_settings->GetBoolean(printing::kSettingPreviewModifiable,
                                  &source_is_html)) {
      NOTREACHED();
    }
  } else {
    source_is_html = !PrintingNodeOrPdfFrame(frame, node);
  }

  if (print_for_preview_ || !source_is_html) {
     modified_job_settings.MergeDictionary(job_settings);
     modified_job_settings.SetBoolean(printing::kSettingHeaderFooterEnabled,
                                      false);
#if defined(OS_MACOSX) && !defined(USE_SKIA)
    bool get_margins_from_pdf = !source_is_html && !print_for_preview_;
#elif defined(OS_WIN) || defined(OS_MACOSX)
    bool get_margins_from_pdf = !source_is_html && print_for_preview_;
#else
    bool get_margins_from_pdf = false;
#endif
    printing::MarginType margin_type = printing::NO_MARGINS;
    if (get_margins_from_pdf)
      margin_type = GetMarginsForPdf(frame, node);
     modified_job_settings.SetInteger(printing::kSettingMarginsType,
                                     margin_type);
     job_settings = &modified_job_settings;
   }
 
  int cookie = print_pages_params_.get() ?
      print_pages_params_->params.document_cookie : 0;
  PrintMsg_PrintPages_Params settings;
  Send(new PrintHostMsg_UpdatePrintSettings(routing_id(),
      cookie, *job_settings, &settings));
  print_pages_params_.reset(new PrintMsg_PrintPages_Params(settings));

  if (!PrintMsg_Print_Params_IsValid(settings.params)) {
    if (!print_for_preview_) {
      print_preview_context_.set_error(PREVIEW_ERROR_INVALID_PRINTER_SETTINGS);
    } else {
      WebKit::WebFrame* print_frame = NULL;
      GetPrintFrame(&print_frame);
      if (print_frame) {
        render_view()->RunModalAlertDialog(
            print_frame,
            l10n_util::GetStringUTF16(
                IDS_PRINT_PREVIEW_INVALID_PRINTER_SETTINGS));
      }
    }
    return false;
  }

  if (settings.params.dpi < kMinDpi || !settings.params.document_cookie) {
    print_preview_context_.set_error(PREVIEW_ERROR_UPDATING_PRINT_SETTINGS);
    return false;
  }

  if (!print_for_preview_) {
    if (!job_settings->GetString(printing::kPreviewUIAddr,
                                 &(settings.params.preview_ui_addr)) ||
        !job_settings->GetInteger(printing::kPreviewRequestID,
                                  &(settings.params.preview_request_id)) ||
        !job_settings->GetBoolean(printing::kIsFirstRequest,
                                  &(settings.params.is_first_request))) {
      NOTREACHED();
      print_preview_context_.set_error(PREVIEW_ERROR_BAD_SETTING);
      return false;
     }
 
     settings.params.print_to_pdf = IsPrintToPdfRequested(*job_settings);
     UpdateFrameMarginsCssInfo(*job_settings);
 
    settings.params.fit_to_paper_size = source_is_html &&
                                        !IsPrintToPdfRequested(*job_settings);
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
