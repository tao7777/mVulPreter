void WebstoreStandaloneInstaller::BeginInstall() {
   AddRef();
 
   if (!crx_file::id_util::IdIsValid(id_)) {
    CompleteInstall(webstore_install::INVALID_ID,
                    webstore_install::kInvalidWebstoreItemId);
     return;
   }
 
  webstore_install::Result result = webstore_install::OTHER_ERROR;
  std::string error;
  if (!EnsureUniqueInstall(&result, &error)) {
    CompleteInstall(result, error);
    return;
  }

  webstore_data_fetcher_.reset(new WebstoreDataFetcher(
      this,
      profile_->GetRequestContext(),
      GetRequestorURL(),
      id_));
  webstore_data_fetcher_->Start();
}
