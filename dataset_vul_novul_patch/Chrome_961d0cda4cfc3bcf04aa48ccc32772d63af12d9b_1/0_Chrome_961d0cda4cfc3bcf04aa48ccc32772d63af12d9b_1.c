 void ManualFillingControllerImpl::CreateForWebContentsForTesting(
     content::WebContents* web_contents,
     base::WeakPtr<PasswordAccessoryController> pwd_controller,
    PasswordGenerationController* pwd_generation_controller_for_testing,
     std::unique_ptr<ManualFillingViewInterface> view) {
   DCHECK(web_contents) << "Need valid WebContents to attach controller to!";
   DCHECK(!FromWebContents(web_contents)) << "Controller already attached!";
   DCHECK(pwd_controller);
  DCHECK(pwd_generation_controller_for_testing);
   DCHECK(view);
 
   web_contents->SetUserData(
       UserDataKey(),
      base::WrapUnique(new ManualFillingControllerImpl(
          web_contents, std::move(pwd_controller),
          pwd_generation_controller_for_testing, std::move(view))));
 }
