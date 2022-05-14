void PrintJobWorker::GetSettingsWithUI(
    int document_page_count,
    bool has_selection,
     bool is_scripted) {
   DCHECK_CURRENTLY_ON(BrowserThread::UI);
 
  PrintingContextDelegate* printing_context_delegate =
      static_cast<PrintingContextDelegate*>(printing_context_delegate_.get());
  content::WebContents* web_contents =
      printing_context_delegate->GetWebContents();

 #if defined(OS_ANDROID)
   if (is_scripted) {
     TabAndroid* tab =
         web_contents ? TabAndroid::FromWebContents(web_contents) : nullptr;
 
    if (tab)
      tab->SetPendingPrint();
   }
 #endif
 
  // Running a dialog causes an exit to webpage-initiated fullscreen.
  // http://crbug.com/728276
  if (web_contents->IsFullscreenForCurrentTab())
    web_contents->ExitFullscreen(true);

   printing_context_->AskUserForSettings(
       document_page_count, has_selection, is_scripted,
      base::Bind(&PostOnOwnerThread, make_scoped_refptr(owner_),
                 base::Bind(&PrintJobWorker::GetSettingsDone,
                            weak_factory_.GetWeakPtr())));
}
