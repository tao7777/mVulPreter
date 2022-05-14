SSLErrorHandler::SSLErrorHandler(base::WeakPtr<Delegate> delegate,
SSLErrorHandler::SSLErrorHandler(const base::WeakPtr<Delegate>& delegate,
                                  const content::GlobalRequestID& id,
                                  ResourceType::Type resource_type,
                                  const GURL& url,
                                 int render_process_id,
                                 int render_view_id)
    : manager_(NULL),
      request_id_(id),
      delegate_(delegate),
      render_process_id_(render_process_id),
      render_view_id_(render_view_id),
      request_url_(url),
      resource_type_(resource_type),
      request_has_been_notified_(false) {
  DCHECK(!BrowserThread::CurrentlyOn(BrowserThread::UI));
  DCHECK(delegate);

  AddRef();
}
