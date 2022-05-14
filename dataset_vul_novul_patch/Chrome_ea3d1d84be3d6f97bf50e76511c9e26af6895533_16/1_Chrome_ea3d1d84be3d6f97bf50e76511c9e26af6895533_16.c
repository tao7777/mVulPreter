void WebPluginDelegateProxy::OnGetPluginElement(
    int route_id, bool* success, intptr_t* npobject_ptr) {
   *success = false;
   NPObject* npobject = NULL;
   if (plugin_)
    npobject = plugin_->GetPluginElement();
  if (!npobject)
    return;

   new NPObjectStub(
       npobject, channel_host_.get(), route_id, 0, page_url_);
   *success = true;
  *npobject_ptr = reinterpret_cast<intptr_t>(npobject);
 }
