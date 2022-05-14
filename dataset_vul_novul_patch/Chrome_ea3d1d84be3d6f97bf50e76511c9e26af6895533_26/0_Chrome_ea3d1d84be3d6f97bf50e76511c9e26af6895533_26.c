void PluginInstance::DidFinishLoadWithReason(const GURL& url, NPReason reason,
void PluginInstance::DidFinishLoadWithReason(
    const GURL& url, NPReason reason, int notify_id) {
  bool notify;
  void* notify_data;
  GetNotifyData(notify_id, &notify, &notify_data);
  if (!notify) {
    NOTREACHED();
    return;
  }

   NPP_URLNotify(url.spec().c_str(), reason, notify_data);
 }
