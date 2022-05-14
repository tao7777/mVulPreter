 void PluginInstance::SendJavaScriptStream(const GURL& url,
                                           const std::string& result,
                                           bool success,
                                          int notify_id) {
  bool notify;
  void* notify_data;
  GetNotifyData(notify_id, &notify, &notify_data);

   if (success) {
     PluginStringStream *stream =
        new PluginStringStream(this, url, notify, notify_data);
     AddStream(stream);
     stream->SendToPlugin(result, "text/html");
   } else {
    if (notify)
      NPP_URLNotify(url.spec().c_str(), NPRES_DONE, notify_data);
   }
 }
