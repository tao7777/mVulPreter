void NetworkHandler::DeleteCookies(
    const std::string& name,
    Maybe<std::string> url,
     Maybe<std::string> domain,
     Maybe<std::string> path,
     std::unique_ptr<DeleteCookiesCallback> callback) {
  if (!storage_partition_) {
     callback->sendFailure(Response::InternalError());
     return;
   }

  if (!url.isJust() && !domain.isJust()) {
    callback->sendFailure(Response::InvalidParams(
        "At least one of the url and domain needs to be specified"));
  }
  BrowserThread::PostTask(
       BrowserThread::IO, FROM_HERE,
       base::BindOnce(
           &DeleteCookiesOnIO,
          base::Unretained(storage_partition_->GetURLRequestContext()), name,
          url.fromMaybe(""), domain.fromMaybe(""), path.fromMaybe(""),
           base::BindOnce(&DeleteCookiesCallback::sendSuccess,
                          std::move(callback))));
 }
