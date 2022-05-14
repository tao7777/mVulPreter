 bool ChildProcessSecurityPolicyImpl::CanCommitURL(int child_id,
                                                  const GURL& url) {
   if (!url.is_valid())
     return false;  // Can't commit invalid URLs.
 
  const std::string& scheme = url.scheme();

  if (IsPseudoScheme(scheme))
    return url == url::kAboutBlankURL || url == kAboutSrcDocURL;

  if (url.SchemeIsBlob() || url.SchemeIsFileSystem()) {
    if (IsMalformedBlobUrl(url))
       return false;
 
     url::Origin origin = url::Origin::Create(url);
    return origin.unique() || CanCommitURL(child_id, GURL(origin.Serialize()));
   }
 
   {
     base::AutoLock lock(lock_);
 
     if (base::ContainsKey(schemes_okay_to_commit_in_any_process_, scheme))
       return true;

    SecurityStateMap::iterator state = security_state_.find(child_id);
    if (state == security_state_.end())
      return false;

    return state->second->CanCommitURL(url);
   }
 }
