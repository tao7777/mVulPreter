 void NetworkHandler::GetAllCookies(
     std::unique_ptr<GetAllCookiesCallback> callback) {
  if (!process_) {
     callback->sendFailure(Response::InternalError());
     return;
   }

  scoped_refptr<CookieRetriever> retriever =
      new CookieRetriever(std::move(callback));

  BrowserThread::PostTask(
       BrowserThread::IO, FROM_HERE,
       base::BindOnce(
           &CookieRetriever::RetrieveAllCookiesOnIO, retriever,
          base::Unretained(
              process_->GetStoragePartition()->GetURLRequestContext())));
 }
