void HttpBridge::MakeAsynchronousPost() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  base::AutoLock lock(fetch_state_lock_);
  DCHECK(!fetch_state_.request_completed);
   if (fetch_state_.aborted)
     return;
 
  fetch_state_.url_poster = URLFetcher::Create(0, url_for_request_,
                                               URLFetcher::POST, this);
   fetch_state_.url_poster->set_request_context(context_getter_for_request_);
   fetch_state_.url_poster->set_upload_data(content_type_, request_content_);
   fetch_state_.url_poster->set_extra_request_headers(extra_headers_);
  fetch_state_.url_poster->set_load_flags(net::LOAD_DO_NOT_SEND_COOKIES);
  fetch_state_.url_poster->Start();
}
