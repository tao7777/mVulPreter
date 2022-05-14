PrintViewManagerBase::PrintViewManagerBase(content::WebContents* web_contents)
    : PrintManager(web_contents),
      printing_rfh_(nullptr),
      printing_succeeded_(false),
      inside_inner_message_loop_(false),
 #if !defined(OS_MACOSX)
       expecting_first_page_(true),
 #endif
      queue_(g_browser_process->print_job_manager()->queue()),
      weak_ptr_factory_(this) {
   DCHECK(queue_.get());
   Profile* profile =
       Profile::FromBrowserContext(web_contents->GetBrowserContext());
  printing_enabled_.Init(
      prefs::kPrintingEnabled, profile->GetPrefs(),
      base::Bind(&PrintViewManagerBase::UpdatePrintingEnabled,
                 base::Unretained(this)));
}
