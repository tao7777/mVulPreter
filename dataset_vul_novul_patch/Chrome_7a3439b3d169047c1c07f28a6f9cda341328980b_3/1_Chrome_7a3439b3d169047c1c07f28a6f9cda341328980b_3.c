bool PrintWebViewHelper::GetPrintSettingsFromUser(WebKit::WebFrame* frame,
                                                  const WebKit::WebNode& node,
                                                  int expected_pages_count,
                                                  bool use_browser_overlays) {
  PrintHostMsg_ScriptedPrint_Params params;
  PrintMsg_PrintPages_Params print_settings;

  params.host_window_id = render_view()->GetHostWindow();
  params.cookie = print_pages_params_->params.document_cookie;
  params.has_selection = frame->hasSelection();
  params.expected_pages_count = expected_pages_count;
  printing::MarginType margin_type = printing::DEFAULT_MARGINS;
  if (PrintingNodeOrPdfFrame(frame, node))
    margin_type = GetMarginsForPdf(frame, node);
  params.margin_type = margin_type;
 
   Send(new PrintHostMsg_DidShowPrintDialog(routing_id()));
 
   print_pages_params_.reset();
   IPC::SyncMessage* msg =
       new PrintHostMsg_ScriptedPrint(routing_id(), params, &print_settings);
   msg->EnableMessagePumping();
   Send(msg);
   print_pages_params_.reset(new PrintMsg_PrintPages_Params(print_settings));
   return (print_settings.params.dpi && print_settings.params.document_cookie);
 }
