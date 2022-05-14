 void PasswordAccessoryControllerImpl::CreateForWebContentsForTesting(
     content::WebContents* web_contents,
     base::WeakPtr<ManualFillingController> mf_controller,
     favicon::FaviconService* favicon_service) {
   DCHECK(web_contents) << "Need valid WebContents to attach controller to!";
   DCHECK(!FromWebContents(web_contents)) << "Controller already attached!";
   DCHECK(mf_controller);
 
   web_contents->SetUserData(
      UserDataKey(),
      base::WrapUnique(new PasswordAccessoryControllerImpl(
          web_contents, std::move(mf_controller), favicon_service)));
 }
