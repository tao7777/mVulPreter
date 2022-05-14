 PasswordAccessoryControllerImpl::PasswordAccessoryControllerImpl(
     content::WebContents* web_contents)
     : web_contents_(web_contents),
      create_dialog_factory_(
          base::BindRepeating(&PasswordGenerationDialogViewInterface::Create)),
       favicon_service_(FaviconServiceFactory::GetForProfile(
           Profile::FromBrowserContext(web_contents->GetBrowserContext()),
           ServiceAccessType::EXPLICIT_ACCESS)) {
}
