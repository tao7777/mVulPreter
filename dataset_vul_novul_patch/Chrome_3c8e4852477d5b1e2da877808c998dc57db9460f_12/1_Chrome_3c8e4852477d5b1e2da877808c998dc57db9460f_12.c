 void NetworkHandler::ClearBrowserCookies(
     std::unique_ptr<ClearBrowserCookiesCallback> callback) {
  if (!process_) {
     callback->sendFailure(Response::InternalError());
     return;
   }

  BrowserThread::PostTask(
       BrowserThread::IO, FROM_HERE,
       base::BindOnce(
           &ClearCookiesOnIO,
          base::Unretained(
              process_->GetStoragePartition()->GetURLRequestContext()),
           std::move(callback)));
 }
