 void ExtensionHelper::InlineWebstoreInstall(
    int install_id, std::string webstore_item_id, const GURL& requestor_url) {
   Send(new ExtensionHostMsg_InlineWebstoreInstall(
       routing_id(), install_id, webstore_item_id, requestor_url));
 }
