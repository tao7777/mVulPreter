 void PrintJobWorker::GetSettings(bool ask_user_for_settings,
                                 gfx::NativeWindow parent_window,
                                  int document_page_count,
                                  bool has_selection,
                                  bool use_overlays) {
  DCHECK_EQ(message_loop(), MessageLoop::current());
  DCHECK_EQ(page_number_, PageNumber::npos());

  MessageLoop::current()->SetNestableTasksAllowed(true);
  printing_context_.SetUseOverlays(use_overlays);

  if (ask_user_for_settings) {
#if defined(OS_MACOSX)
     ChromeThread::PostTask(
         ChromeThread::UI, FROM_HERE,
         NewRunnableMethod(this, &PrintJobWorker::GetSettingsWithUI,
                          parent_window, document_page_count,
                           has_selection));
 #else
     PrintingContext::Result result = printing_context_.AskUserForSettings(
        parent_window, document_page_count, has_selection);
     GetSettingsDone(result);
 #endif
   } else {
    PrintingContext::Result result = printing_context_.UseDefaultSettings();
    GetSettingsDone(result);
  }
}
