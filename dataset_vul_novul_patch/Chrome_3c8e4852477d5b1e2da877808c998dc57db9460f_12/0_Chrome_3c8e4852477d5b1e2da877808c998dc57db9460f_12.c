 void NetworkHandler::ClearBrowserCookies(
     std::unique_ptr<ClearBrowserCookiesCallback> callback) {
  if (!storage_partition_) {
     callback->sendFailure(Response::InternalError());
     return;
   }

  BrowserThread::PostTask(
       BrowserThread::IO, FROM_HERE,
       base::BindOnce(
           &ClearCookiesOnIO,
          base::Unretained(storage_partition_->GetURLRequestContext()),
           std::move(callback)));
 }
