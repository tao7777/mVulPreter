void ShowExtensionInstallDialogImpl(
    ExtensionInstallPromptShowParams* show_params,
    ExtensionInstallPrompt::Delegate* delegate,
    scoped_refptr<ExtensionInstallPrompt::Prompt> prompt) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  ExtensionInstallDialogView* dialog =
      new ExtensionInstallDialogView(show_params->profile(),
                                      show_params->GetParentWebContents(),
                                      delegate,
                                      prompt);
  if (prompt->ShouldUseTabModalDialog()) {
    content::WebContents* parent_web_contents =
        show_params->GetParentWebContents();
    if (parent_web_contents)
      constrained_window::ShowWebModalDialogViews(dialog, parent_web_contents);
  } else {
    constrained_window::CreateBrowserModalDialogViews(
        dialog, show_params->GetParentWindow())
        ->Show();
  }
 }
