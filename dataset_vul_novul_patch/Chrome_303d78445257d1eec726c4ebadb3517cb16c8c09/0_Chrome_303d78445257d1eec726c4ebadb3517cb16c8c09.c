ExtensionInstallDialogView::ExtensionInstallDialogView(
    Profile* profile,
    content::PageNavigator* navigator,
    const ExtensionInstallPrompt::DoneCallback& done_callback,
    std::unique_ptr<ExtensionInstallPrompt::Prompt> prompt)
    : profile_(profile),
      navigator_(navigator),
      done_callback_(done_callback),
       prompt_(std::move(prompt)),
       container_(NULL),
       scroll_view_(NULL),
      handled_result_(false),
      install_button_enabled_(false) {
   InitView();
 }
