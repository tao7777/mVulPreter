 void NetworkHandler::SetCookies(
     std::unique_ptr<protocol::Array<Network::CookieParam>> cookies,
     std::unique_ptr<SetCookiesCallback> callback) {
  if (!process_) {
     callback->sendFailure(Response::InternalError());
     return;
   }

  BrowserThread::PostTask(
       BrowserThread::IO, FROM_HERE,
       base::BindOnce(
           &SetCookiesOnIO,
          base::Unretained(
              process_->GetStoragePartition()->GetURLRequestContext()),
           std::move(cookies),
           base::BindOnce(&CookiesSetOnIO, std::move(callback))));
 }
