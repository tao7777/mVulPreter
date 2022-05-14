bool PrintWebViewHelper::InitPrintSettings(WebKit::WebFrame* frame,
                                           WebKit::WebNode* node,
                                           bool is_preview) {
   DCHECK(frame);
   PrintMsg_PrintPages_Params settings;
 
  Send(new PrintHostMsg_GetDefaultPrintSettings(routing_id(),
                                                &settings.params));
   bool result = true;
   if (PrintMsg_Print_Params_IsEmpty(settings.params)) {
    if (!is_preview) {
      render_view()->runModalAlertDialog(
          frame,
          l10n_util::GetStringUTF16(
              IDS_PRINT_PREVIEW_INVALID_PRINTER_SETTINGS));
    }
     result = false;
   }
 
  if (result &&
      (settings.params.dpi < kMinDpi || settings.params.document_cookie == 0)) {
    NOTREACHED();
    result = false;
  }

  settings.pages.clear();
  print_pages_params_.reset(new PrintMsg_PrintPages_Params(settings));
  return result;
}
