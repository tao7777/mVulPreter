 void WebPluginDelegateProxy::OnGetWindowScriptNPObject(
    int route_id, bool* success) {
   *success = false;
   NPObject* npobject = NULL;
   if (plugin_)
    npobject = plugin_->GetWindowScriptNPObject();

  if (!npobject)
    return;

   window_script_object_ = (new NPObjectStub(
       npobject, channel_host_.get(), route_id, 0, page_url_))->AsWeakPtr();
   *success = true;
 }
