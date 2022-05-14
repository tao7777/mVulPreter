void WebPluginDelegateStub::OnGetPluginScriptableObject(
void WebPluginDelegateStub::OnGetPluginScriptableObject(int* route_id) {
   NPObject* object = delegate_->GetPluginScriptableObject();
   if (!object) {
     *route_id = MSG_ROUTING_NONE;
     return;
   }
 
   *route_id = channel_->GenerateRouteID();
   new NPObjectStub(
      object, channel_.get(), *route_id, webplugin_->containing_window(),
      page_url_);

  WebBindings::releaseObject(object);
}
