 bool ChildProcessSecurityPolicyImpl::CanSetAsOriginHeader(int child_id,
                                                           const GURL& url) {
   if (!url.is_valid())
    return false;  // Can't set invalid URLs as origin headers.

  if (url == kAboutSrcDocURL)
    return false;
 
  //
  // TODO(alexmos): This should eventually also check the origin lock, but
  // currently this is not done due to certain corner cases involving HTML
  // imports and layout tests that simulate requests from isolated worlds.  See
  // https://crbug.com/515309.
  if (CanCommitURL(child_id, url, false /* check_origin_lock */))
     return true;
 
  {
    base::AutoLock lock(lock_);
    if (base::ContainsKey(schemes_okay_to_appear_as_origin_headers_,
                          url.scheme())) {
      return true;
    }
  }
  return false;
}
