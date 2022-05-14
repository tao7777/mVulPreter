 void WebstoreStandaloneInstaller::OnWebstoreRequestFailure() {
   OnWebStoreDataFetcherDone();
   CompleteInstall(webstore_install::WEBSTORE_REQUEST_ERROR,
                  webstore_install::kWebstoreRequestError);
 }
