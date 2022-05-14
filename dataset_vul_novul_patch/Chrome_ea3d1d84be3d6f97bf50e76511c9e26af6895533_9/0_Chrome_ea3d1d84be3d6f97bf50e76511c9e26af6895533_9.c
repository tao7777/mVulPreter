 WebPluginResourceClient* WebPluginDelegatePepper::CreateResourceClient(
    unsigned long resource_id, const GURL& url, int notify_id) {
  return instance()->CreateStream(resource_id, url, std::string(), notify_id);
}
 
WebPluginResourceClient* WebPluginDelegatePepper::CreateSeekableResourceClient(
    unsigned long resource_id, int range_request_id) {
  return instance()->GetRangeRequest(range_request_id);
 }
