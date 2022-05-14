WebContents* PrintPreviewDialogController::CreatePrintPreviewDialog(
    WebContents* initiator) {
  base::AutoReset<bool> auto_reset(&is_creating_print_preview_dialog_, true);

  ConstrainedWebDialogDelegate* web_dialog_delegate =
      ShowConstrainedWebDialog(initiator->GetBrowserContext(),
                               new PrintPreviewDialogDelegate(initiator),
                               initiator);

  WebContents* preview_dialog = web_dialog_delegate->GetWebContents();

  GURL print_url(chrome::kChromeUIPrintURL);
   content::HostZoomMap::Get(preview_dialog->GetSiteInstance())
       ->SetZoomLevelForHostAndScheme(print_url.scheme(), print_url.host(), 0);
   PrintViewManager::CreateForWebContents(preview_dialog);
   extensions::ChromeExtensionWebContentsObserver::CreateForWebContents(
       preview_dialog);
 
  preview_dialog_map_[preview_dialog] = initiator;
  waiting_for_new_preview_page_ = true;

  task_manager::WebContentsTags::CreateForPrintingContents(preview_dialog);

  AddObservers(initiator);
  AddObservers(preview_dialog);

  return preview_dialog;
}
