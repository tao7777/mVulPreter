void TabHelper::OnInlineWebstoreInstall(content::RenderFrameHost* host,
                                        int install_id,
                                        int return_route_id,
                                        const std::string& webstore_item_id,
                                        const GURL& requestor_url,
                                        int listeners_mask) {
  if ((listeners_mask & ~(api::webstore::INSTALL_STAGE_LISTENER |
                          api::webstore::DOWNLOAD_PROGRESS_LISTENER)) != 0 ||
      requestor_url.is_empty()) {
     NOTREACHED();
     return;
   }
   ExtensionRegistry* registry = ExtensionRegistry::Get(profile_);
  if (registry->disabled_extensions().Contains(webstore_item_id) &&
      (ExtensionPrefs::Get(profile_)->GetDisableReasons(webstore_item_id) &
           Extension::DISABLE_PERMISSIONS_INCREASE) != 0) {
       extension_reenabler_.reset();
       extension_reenabler_ = ExtensionReenabler::PromptForReenable(
          registry->disabled_extensions().GetByID(webstore_item_id),
          profile_,
          web_contents(),
          requestor_url,
           base::Bind(&TabHelper::OnReenableComplete,
                     weak_ptr_factory_.GetWeakPtr(),
                     install_id,
                     return_route_id));
   } else {
    WebstoreAPI::Get(profile_)->OnInlineInstallStart(
        return_route_id, this, webstore_item_id, listeners_mask);
    WebstoreStandaloneInstaller::Callback callback =
        base::Bind(&TabHelper::OnInlineInstallComplete,
                   base::Unretained(this),
                   install_id,
                   return_route_id);
     scoped_refptr<WebstoreInlineInstaller> installer(
         webstore_inline_installer_factory_->CreateInstaller(
             web_contents(), host, webstore_item_id, requestor_url, callback));
    installer->BeginInstall();
  }
}
