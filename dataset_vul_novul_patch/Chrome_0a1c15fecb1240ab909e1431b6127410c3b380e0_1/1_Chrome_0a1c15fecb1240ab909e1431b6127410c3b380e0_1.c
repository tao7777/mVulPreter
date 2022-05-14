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
  constrained_window::CreateBrowserModalDialogViews(
      dialog, show_params->GetParentWindow())->Show();
 }
