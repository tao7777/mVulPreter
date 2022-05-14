 PasswordAccessoryControllerImpl::PasswordAccessoryControllerImpl(
     content::WebContents* web_contents,
     base::WeakPtr<ManualFillingController> mf_controller,
    CreateDialogFactory create_dialog_factory,
     favicon::FaviconService* favicon_service)
     : web_contents_(web_contents),
       mf_controller_(std::move(mf_controller)),
      create_dialog_factory_(create_dialog_factory),
       favicon_service_(favicon_service) {}
