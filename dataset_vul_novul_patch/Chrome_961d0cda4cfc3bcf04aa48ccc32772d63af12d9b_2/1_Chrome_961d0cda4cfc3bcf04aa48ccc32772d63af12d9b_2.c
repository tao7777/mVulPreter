 ManualFillingControllerImpl::ManualFillingControllerImpl(
     content::WebContents* web_contents,
     base::WeakPtr<PasswordAccessoryController> pwd_controller,
     std::unique_ptr<ManualFillingViewInterface> view)
     : web_contents_(web_contents),
       pwd_controller_(std::move(pwd_controller)),
       view_(std::move(view)),
       weak_factory_(this) {}
