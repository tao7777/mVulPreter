SocketStreamDispatcherHost::SocketStreamDispatcherHost(
     int render_process_id,
     ResourceMessageFilter::URLRequestContextSelector* selector,
     content::ResourceContext* resource_context)
    : ALLOW_THIS_IN_INITIALIZER_LIST(ssl_delegate_weak_factory_(this)),
      render_process_id_(render_process_id),
       url_request_context_selector_(selector),
       resource_context_(resource_context) {
   DCHECK(selector);
  net::WebSocketJob::EnsureInit();
}
