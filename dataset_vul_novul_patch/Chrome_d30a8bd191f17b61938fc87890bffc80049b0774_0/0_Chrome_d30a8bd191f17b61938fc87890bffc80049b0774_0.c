 void TabHelper::OnInlineInstallComplete(int install_id,
                                         int return_route_id,
                                        const std::string& extension_id,
                                         bool success,
                                         const std::string& error,
                                         webstore_install::Result result) {
  DCHECK_EQ(1u, pending_inline_installations_.count(extension_id));
  pending_inline_installations_.erase(extension_id);
  install_observers_.erase(extension_id);
   Send(new ExtensionMsg_InlineWebstoreInstallResponse(
       return_route_id,
       install_id,
      success,
      success ? std::string() : error,
      result));
}
