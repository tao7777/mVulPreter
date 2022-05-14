 bool PrintWebViewHelper::CheckForCancel() {
   bool cancel = false;
  Send(new PrintHostMsg_CheckForCancel(
      routing_id(),
      print_pages_params_->params.preview_ui_addr,
      print_pages_params_->params.preview_request_id,
      &cancel));
   if (cancel)
     notify_browser_of_print_failure_ = false;
   return cancel;
}
