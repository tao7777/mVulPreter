void NetworkHandler::GetCookies(Maybe<Array<String>> protocol_urls,
                                std::unique_ptr<GetCookiesCallback> callback) {
  if (!host_) {
    callback->sendFailure(Response::InternalError());
    return;
  }

  std::vector<GURL> urls = ComputeCookieURLs(host_, protocol_urls);
  scoped_refptr<CookieRetriever> retriever =
      new CookieRetriever(std::move(callback));

  BrowserThread::PostTask(
       BrowserThread::IO, FROM_HERE,
       base::BindOnce(
           &CookieRetriever::RetrieveCookiesOnIO, retriever,
          base::Unretained(storage_partition_->GetURLRequestContext()), urls));
 }
