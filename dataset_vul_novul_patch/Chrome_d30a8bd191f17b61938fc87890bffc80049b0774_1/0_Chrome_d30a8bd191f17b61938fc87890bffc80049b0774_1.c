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

  if (pending_inline_installations_.count(webstore_item_id) != 0) {
    Send(new ExtensionMsg_InlineWebstoreInstallResponse(
        return_route_id, install_id, false,
        webstore_install::kInstallInProgressError,
        webstore_install::INSTALL_IN_PROGRESS));
    return;
  }

  pending_inline_installations_.insert(webstore_item_id);
   ExtensionRegistry* registry = ExtensionRegistry::Get(profile_);
  if (registry->disabled_extensions().Contains(webstore_item_id) &&
      (ExtensionPrefs::Get(profile_)->GetDisableReasons(webstore_item_id) &
           Extension::DISABLE_PERMISSIONS_INCREASE) != 0) {
       extension_reenabler_.reset();
       extension_reenabler_ = ExtensionReenabler::PromptForReenable(
          registry->disabled_extensions().GetByID(webstore_item_id), profile_,
          web_contents(), requestor_url,
           base::Bind(&TabHelper::OnReenableComplete,
                     weak_ptr_factory_.GetWeakPtr(), install_id,
                     return_route_id, webstore_item_id));
   } else {
    bool observe_download_progress =
        (listeners_mask & api::webstore::DOWNLOAD_PROGRESS_LISTENER) != 0;
    bool observe_install_stage =
        (listeners_mask & api::webstore::INSTALL_STAGE_LISTENER) != 0;
    if (observe_install_stage || observe_download_progress) {
      DCHECK_EQ(0u, install_observers_.count(webstore_item_id));
      install_observers_[webstore_item_id] =
          base::MakeUnique<InlineInstallObserver>(
              this, web_contents()->GetBrowserContext(), return_route_id,
              webstore_item_id, observe_download_progress,
              observe_install_stage);
    }

    WebstoreStandaloneInstaller::Callback callback = base::Bind(
        &TabHelper::OnInlineInstallComplete, weak_ptr_factory_.GetWeakPtr(),
        install_id, return_route_id, webstore_item_id);
     scoped_refptr<WebstoreInlineInstaller> installer(
         webstore_inline_installer_factory_->CreateInstaller(
             web_contents(), host, webstore_item_id, requestor_url, callback));
    installer->BeginInstall();
  }
}
