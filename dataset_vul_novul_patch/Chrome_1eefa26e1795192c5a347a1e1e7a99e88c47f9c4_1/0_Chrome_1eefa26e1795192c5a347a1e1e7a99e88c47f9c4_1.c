   bool CanCommitURL(const GURL& url) {
    // Having permission to a scheme implies permission to all of its URLs.
    SchemeMap::const_iterator scheme_judgment(
        scheme_policy_.find(url.scheme()));
    if (scheme_judgment != scheme_policy_.end())
      return scheme_judgment->second;

    // Otherwise, check for permission for specific origin.
    if (ContainsKey(origin_set_, url::Origin(url)))
      return true;
 
    if (url.SchemeIs(url::kFileScheme)) {
      base::FilePath path;
      if (net::FileURLToFilePath(url, &path))
        return ContainsKey(request_file_set_, path);
    }

    return false;  // Unmentioned schemes are disallowed.
  }
