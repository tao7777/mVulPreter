void WebPluginDelegateStub::OnGetPluginScriptableObject(
    int* route_id,
    intptr_t* npobject_ptr) {
   NPObject* object = delegate_->GetPluginScriptableObject();
   if (!object) {
     *route_id = MSG_ROUTING_NONE;
     return;
   }
 
   *route_id = channel_->GenerateRouteID();
  *npobject_ptr = reinterpret_cast<intptr_t>(object);
   new NPObjectStub(
      object, channel_.get(), *route_id, webplugin_->containing_window(),
      page_url_);

  WebBindings::releaseObject(object);
}
