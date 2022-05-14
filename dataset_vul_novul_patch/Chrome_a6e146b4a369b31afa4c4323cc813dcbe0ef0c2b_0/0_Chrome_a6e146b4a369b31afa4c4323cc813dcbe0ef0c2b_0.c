 URLFetcher* FakeURLFetcherFactory::CreateURLFetcher(
    int id,
    const GURL& url,
    URLFetcher::RequestType request_type,
     URLFetcher::Delegate* d) {
   FakeResponseMap::const_iterator it = fake_responses_.find(url);
   if (it == fake_responses_.end()) {
    if (default_factory_ == NULL) {
      // If we don't have a baked response for that URL we return NULL.
      DLOG(ERROR) << "No baked response for URL: " << url.spec();
      return NULL;
    } else {
      return default_factory_->CreateURLFetcher(id, url, request_type, d);
    }
   }
   return new FakeURLFetcher(url, request_type, d,
                             it->second.first, it->second.second);
}
