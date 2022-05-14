 void WebstoreStandaloneInstaller::OnWebstoreResponseParseSuccess(
    std::unique_ptr<base::DictionaryValue> webstore_data) {
  OnWebStoreDataFetcherDone();

  if (!CheckRequestorAlive()) {
    CompleteInstall(webstore_install::ABORTED, std::string());
    return;
  }

  std::string error;

  if (!CheckInlineInstallPermitted(*webstore_data, &error)) {
    CompleteInstall(webstore_install::NOT_PERMITTED, error);
    return;
  }

  if (!CheckRequestorPermitted(*webstore_data, &error)) {
    CompleteInstall(webstore_install::NOT_PERMITTED, error);
    return;
  }

  std::string manifest;
  if (!webstore_data->GetString(kManifestKey, &manifest) ||
      !webstore_data->GetString(kUsersKey, &localized_user_count_) ||
       !webstore_data->GetDouble(kAverageRatingKey, &average_rating_) ||
       !webstore_data->GetInteger(kRatingCountKey, &rating_count_)) {
     CompleteInstall(webstore_install::INVALID_WEBSTORE_RESPONSE,
                    kInvalidWebstoreResponseError);
     return;
   }
 
  show_user_count_ = true;
  webstore_data->GetBoolean(kShowUserCountKey, &show_user_count_);

   if (average_rating_ < ExtensionInstallPrompt::kMinExtensionRating ||
       average_rating_ > ExtensionInstallPrompt::kMaxExtensionRating) {
     CompleteInstall(webstore_install::INVALID_WEBSTORE_RESPONSE,
                    kInvalidWebstoreResponseError);
     return;
   }
 
  if ((webstore_data->HasKey(kLocalizedNameKey) &&
      !webstore_data->GetString(kLocalizedNameKey, &localized_name_)) ||
      (webstore_data->HasKey(kLocalizedDescriptionKey) &&
       !webstore_data->GetString(
           kLocalizedDescriptionKey, &localized_description_))) {
     CompleteInstall(webstore_install::INVALID_WEBSTORE_RESPONSE,
                    kInvalidWebstoreResponseError);
     return;
   }
 
  GURL icon_url;
  if (webstore_data->HasKey(kIconUrlKey)) {
     std::string icon_url_string;
     if (!webstore_data->GetString(kIconUrlKey, &icon_url_string)) {
       CompleteInstall(webstore_install::INVALID_WEBSTORE_RESPONSE,
                      kInvalidWebstoreResponseError);
       return;
     }
     icon_url = GURL(extension_urls::GetWebstoreLaunchURL()).Resolve(
         icon_url_string);
     if (!icon_url.is_valid()) {
       CompleteInstall(webstore_install::INVALID_WEBSTORE_RESPONSE,
                      kInvalidWebstoreResponseError);
       return;
     }
   }

  webstore_data_ = std::move(webstore_data);

  scoped_refptr<WebstoreInstallHelper> helper =
      new WebstoreInstallHelper(this,
                                id_,
                                manifest,
                                icon_url,
                                profile_->GetRequestContext());
  helper->Start();
}
