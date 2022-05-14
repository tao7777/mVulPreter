 bool ChildProcessSecurityPolicyImpl::CanCommitURL(int child_id,
                                                  const GURL& url,
                                                  bool check_origin_locks) {
   if (!url.is_valid())
     return false;  // Can't commit invalid URLs.
 
  const std::string& scheme = url.scheme();

  if (IsPseudoScheme(scheme))
    return url == url::kAboutBlankURL || url == kAboutSrcDocURL;

  if (url.SchemeIsBlob() || url.SchemeIsFileSystem()) {
    if (IsMalformedBlobUrl(url))
       return false;
 
     url::Origin origin = url::Origin::Create(url);
    return origin.unique() ||
           CanCommitURL(child_id, GURL(origin.Serialize()), check_origin_locks);
   }
 
  // With site isolation, a URL from a site may only be committed in a process
  // dedicated to that site.  This check will ensure that |url| can't commit if
  // the process is locked to a different site.  Note that this check is only
  // effective for processes that are locked to a site, but even with strict
  // site isolation, currently not all processes are locked (e.g., extensions
  // or <webview> tags - see ShouldLockToOrigin()).
  if (check_origin_locks && !CanAccessDataForOrigin(child_id, url))
    return false;

   {
     base::AutoLock lock(lock_);
 
    // TODO(creis, nick): https://crbug.com/515309: The line below does not
     if (base::ContainsKey(schemes_okay_to_commit_in_any_process_, scheme))
       return true;

    SecurityStateMap::iterator state = security_state_.find(child_id);
    if (state == security_state_.end())
      return false;

    return state->second->CanCommitURL(url);
   }
 }
