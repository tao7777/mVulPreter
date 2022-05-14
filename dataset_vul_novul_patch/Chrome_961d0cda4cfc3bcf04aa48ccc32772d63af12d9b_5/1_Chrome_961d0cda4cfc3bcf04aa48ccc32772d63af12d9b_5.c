 void PasswordAccessoryControllerImpl::CreateForWebContentsForTesting(
     content::WebContents* web_contents,
     base::WeakPtr<ManualFillingController> mf_controller,
    CreateDialogFactory create_dialog_factory,
     favicon::FaviconService* favicon_service) {
   DCHECK(web_contents) << "Need valid WebContents to attach controller to!";
   DCHECK(!FromWebContents(web_contents)) << "Controller already attached!";
   DCHECK(mf_controller);
 
   web_contents->SetUserData(
      UserDataKey(), base::WrapUnique(new PasswordAccessoryControllerImpl(
                         web_contents, std::move(mf_controller),
                         create_dialog_factory, favicon_service)));
 }
