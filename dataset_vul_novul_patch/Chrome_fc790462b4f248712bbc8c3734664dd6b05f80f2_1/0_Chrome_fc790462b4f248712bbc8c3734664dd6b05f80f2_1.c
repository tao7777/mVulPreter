 void PrinterQuery::GetSettings(GetSettingsAskParam ask_user_for_settings,
                               gfx::NativeView parent_view,
                                int expected_page_count,
                                bool has_selection,
                                bool use_overlays,
                               CancelableTask* callback) {
  DCHECK_EQ(ui_message_loop_, MessageLoop::current());
  DCHECK(!is_print_dialog_box_shown_);
  DCHECK(!callback_.get());
  DCHECK(worker_.get());
  if (!worker_.get())
    return;
  if (!worker_->message_loop()) {
    if (!worker_->Start()) {
      if (callback) {
        callback->Cancel();
        delete callback;
      }
      NOTREACHED();
      return;
    }
  }

  callback_.reset(callback);
  is_print_dialog_box_shown_ = ask_user_for_settings == ASK_USER;
  worker_->message_loop()->PostTask(FROM_HERE, NewRunnableMethod(
       worker_.get(),
       &PrintJobWorker::GetSettings,
       is_print_dialog_box_shown_,
      parent_view,
       expected_page_count,
       has_selection,
       use_overlays));
}
