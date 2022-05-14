NPObject* WebPluginProxy::GetWindowScriptNPObject() {
  if (window_npobject_)
    return WebBindings::retainObject(window_npobject_);
 
   int npobject_route_id = channel_->GenerateRouteID();
   bool success = false;
   Send(new PluginHostMsg_GetWindowScriptNPObject(
      route_id_, npobject_route_id, &success));
   if (!success)
     return NULL;
 
  window_npobject_ = NPObjectProxy::Create(
      channel_, npobject_route_id, containing_window_, page_url_);

  return window_npobject_;
}
