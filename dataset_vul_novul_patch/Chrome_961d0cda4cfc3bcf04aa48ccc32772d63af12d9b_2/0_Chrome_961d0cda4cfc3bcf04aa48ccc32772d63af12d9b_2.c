 ManualFillingControllerImpl::ManualFillingControllerImpl(
     content::WebContents* web_contents,
     base::WeakPtr<PasswordAccessoryController> pwd_controller,
    PasswordGenerationController* pwd_generation_controller_for_testing,
     std::unique_ptr<ManualFillingViewInterface> view)
     : web_contents_(web_contents),
       pwd_controller_(std::move(pwd_controller)),
      pwd_generation_controller_for_testing_(
          pwd_generation_controller_for_testing),
       view_(std::move(view)),
       weak_factory_(this) {}
