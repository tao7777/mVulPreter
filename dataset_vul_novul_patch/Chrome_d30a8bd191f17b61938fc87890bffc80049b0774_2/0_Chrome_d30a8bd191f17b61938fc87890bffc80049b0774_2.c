 void TabHelper::OnReenableComplete(int install_id,
                                    int return_route_id,
                                   const std::string& extension_id,
                                    ExtensionReenabler::ReenableResult result) {
   webstore_install::Result webstore_result = webstore_install::SUCCESS;
   std::string error;
  switch (result) {
    case ExtensionReenabler::REENABLE_SUCCESS:
      break;  // already set
    case ExtensionReenabler::USER_CANCELED:
      webstore_result = webstore_install::USER_CANCELLED;
      error = "User canceled install.";
      break;
    case ExtensionReenabler::NOT_ALLOWED:
      webstore_result = webstore_install::NOT_PERMITTED;
      error = "Install not permitted.";
      break;
    case ExtensionReenabler::ABORTED:
      webstore_result = webstore_install::ABORTED;
      error = "Aborted due to tab closing.";
       break;
   }
 
  OnInlineInstallComplete(install_id, return_route_id, extension_id,
                          result == ExtensionReenabler::REENABLE_SUCCESS, error,
                           webstore_result);
  // Note: ExtensionReenabler contained the callback with the curried-in
  // |extension_id|; delete it last.
  extension_reenabler_.reset();
 }
