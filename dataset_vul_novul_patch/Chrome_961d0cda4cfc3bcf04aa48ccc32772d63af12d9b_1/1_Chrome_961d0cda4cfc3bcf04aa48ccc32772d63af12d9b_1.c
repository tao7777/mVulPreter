 void ManualFillingControllerImpl::CreateForWebContentsForTesting(
     content::WebContents* web_contents,
     base::WeakPtr<PasswordAccessoryController> pwd_controller,
     std::unique_ptr<ManualFillingViewInterface> view) {
   DCHECK(web_contents) << "Need valid WebContents to attach controller to!";
   DCHECK(!FromWebContents(web_contents)) << "Controller already attached!";
   DCHECK(pwd_controller);
   DCHECK(view);
 
   web_contents->SetUserData(
       UserDataKey(),
      std::make_unique<ManualFillingControllerImpl>(
          web_contents, std::move(pwd_controller), std::move(view)));
 }
