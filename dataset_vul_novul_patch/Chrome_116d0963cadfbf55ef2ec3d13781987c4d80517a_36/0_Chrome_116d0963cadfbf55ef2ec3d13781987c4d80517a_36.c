 bool PrintWebViewHelper::CheckForCancel() {
  const PrintMsg_Print_Params& print_params = print_pages_params_->params;
   bool cancel = false;
  Send(new PrintHostMsg_CheckForCancel(routing_id(),
                                       print_params.preview_ui_id,
                                       print_params.preview_request_id,
                                       &cancel));
   if (cancel)
     notify_browser_of_print_failure_ = false;
   return cancel;
}
