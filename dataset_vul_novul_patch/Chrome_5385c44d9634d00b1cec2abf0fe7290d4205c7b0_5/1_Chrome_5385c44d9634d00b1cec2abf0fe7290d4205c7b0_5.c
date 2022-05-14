 SSLCertErrorHandler::SSLCertErrorHandler(
    base::WeakPtr<Delegate> delegate,
     const content::GlobalRequestID& id,
     ResourceType::Type resource_type,
     const GURL& url,
    int render_process_id,
    int render_view_id,
    const net::SSLInfo& ssl_info,
    bool fatal)
    : SSLErrorHandler(delegate, id, resource_type, url, render_process_id,
          render_view_id),
      ssl_info_(ssl_info),
      cert_error_(net::MapCertStatusToNetError(ssl_info.cert_status)),
      fatal_(fatal) {
}
