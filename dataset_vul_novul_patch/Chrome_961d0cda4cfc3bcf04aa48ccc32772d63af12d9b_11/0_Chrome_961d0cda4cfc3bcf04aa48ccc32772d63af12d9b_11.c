 PasswordAccessoryControllerImpl::PasswordAccessoryControllerImpl(
     content::WebContents* web_contents)
     : web_contents_(web_contents),
       favicon_service_(FaviconServiceFactory::GetForProfile(
           Profile::FromBrowserContext(web_contents->GetBrowserContext()),
           ServiceAccessType::EXPLICIT_ACCESS)) {
}
