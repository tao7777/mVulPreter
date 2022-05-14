NPObject* WebPluginDelegateProxy::GetPluginScriptableObject() {
  if (npobject_)
     return WebBindings::retainObject(npobject_);
 
   int route_id = MSG_ROUTING_NONE;
  intptr_t npobject_ptr;
  Send(new PluginMsg_GetPluginScriptableObject(
      instance_id_, &route_id, &npobject_ptr));
   if (route_id == MSG_ROUTING_NONE)
     return NULL;
 
  npobject_ = NPObjectProxy::Create(
      channel_host_.get(), route_id, 0, page_url_);

  return WebBindings::retainObject(npobject_);
 }
