 void NetworkHandler::GetAllCookies(
     std::unique_ptr<GetAllCookiesCallback> callback) {
  if (!storage_partition_) {
     callback->sendFailure(Response::InternalError());
     return;
   }

  scoped_refptr<CookieRetriever> retriever =
      new CookieRetriever(std::move(callback));

  BrowserThread::PostTask(
       BrowserThread::IO, FROM_HERE,
       base::BindOnce(
           &CookieRetriever::RetrieveAllCookiesOnIO, retriever,
          base::Unretained(storage_partition_->GetURLRequestContext())));
 }
