   bool CanCommitURL(const GURL& url) {
    SchemeMap::const_iterator judgment(scheme_policy_.find(url.scheme()));
    if (judgment != scheme_policy_.end())
      return judgment->second;
 
    if (url.SchemeIs(url::kFileScheme)) {
      base::FilePath path;
      if (net::FileURLToFilePath(url, &path))
        return ContainsKey(request_file_set_, path);
    }

    return false;  // Unmentioned schemes are disallowed.
  }
