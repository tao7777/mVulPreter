 void TabHelper::OnInlineInstallComplete(int install_id,
                                         int return_route_id,
                                         bool success,
                                         const std::string& error,
                                         webstore_install::Result result) {
   Send(new ExtensionMsg_InlineWebstoreInstallResponse(
       return_route_id,
       install_id,
      success,
      success ? std::string() : error,
      result));
}
