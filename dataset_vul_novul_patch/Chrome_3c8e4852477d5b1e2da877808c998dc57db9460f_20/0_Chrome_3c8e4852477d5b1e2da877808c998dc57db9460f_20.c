 void NetworkHandler::SetCookies(
     std::unique_ptr<protocol::Array<Network::CookieParam>> cookies,
     std::unique_ptr<SetCookiesCallback> callback) {
  if (!storage_partition_) {
     callback->sendFailure(Response::InternalError());
     return;
   }

  BrowserThread::PostTask(
       BrowserThread::IO, FROM_HERE,
       base::BindOnce(
           &SetCookiesOnIO,
          base::Unretained(storage_partition_->GetURLRequestContext()),
           std::move(cookies),
           base::BindOnce(&CookiesSetOnIO, std::move(callback))));
 }
