 URLFetcher* FakeURLFetcherFactory::CreateURLFetcher(
    int id,
    const GURL& url,
    URLFetcher::RequestType request_type,
     URLFetcher::Delegate* d) {
   FakeResponseMap::const_iterator it = fake_responses_.find(url);
   if (it == fake_responses_.end()) {
    DLOG(ERROR) << "No baked response for URL: " << url.spec();
    return NULL;
   }
   return new FakeURLFetcher(url, request_type, d,
                             it->second.first, it->second.second);
}
