void PrintJobWorker::GetSettingsWithUI(gfx::NativeWindow parent_window,
void PrintJobWorker::GetSettingsWithUI(gfx::NativeView parent_view,
                                        int document_page_count,
                                        bool has_selection) {
   DCHECK(ChromeThread::CurrentlyOn(ChromeThread::UI));
 
   PrintingContext::Result result = printing_context_.AskUserForSettings(
      parent_view, document_page_count, has_selection);
   message_loop()->PostTask(FROM_HERE, NewRunnableMethod(
       this, &PrintJobWorker::GetSettingsDone, result));
 }
