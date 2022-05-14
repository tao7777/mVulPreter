 PasswordAccessoryControllerImpl::PasswordAccessoryControllerImpl(
     content::WebContents* web_contents,
     base::WeakPtr<ManualFillingController> mf_controller,
     favicon::FaviconService* favicon_service)
     : web_contents_(web_contents),
       mf_controller_(std::move(mf_controller)),
       favicon_service_(favicon_service) {}
