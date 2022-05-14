 bool ChildProcessSecurityPolicyImpl::CanSetAsOriginHeader(int child_id,
                                                           const GURL& url) {
   if (!url.is_valid())
    return false;  // Can't set invalid URLs as origin headers.

  if (url == kAboutSrcDocURL)
    return false;
 
  if (CanCommitURL(child_id, url))
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
