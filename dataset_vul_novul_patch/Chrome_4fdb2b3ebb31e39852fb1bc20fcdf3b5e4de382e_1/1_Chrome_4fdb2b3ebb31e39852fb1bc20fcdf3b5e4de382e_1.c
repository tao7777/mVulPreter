 bool ResourceFetcher::IsPreloadedForTest(const KURL& url) const {
  DCHECK(preloaded_ur_ls_for_test_);
  return preloaded_ur_ls_for_test_->Contains(url.GetString());
 }
