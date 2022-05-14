 void NetworkHandler::SetCookie(const std::string& name,
                               const std::string& value,
                               Maybe<std::string> url,
                               Maybe<std::string> domain,
                               Maybe<std::string> path,
                               Maybe<bool> secure,
                               Maybe<bool> http_only,
                                Maybe<std::string> same_site,
                                Maybe<double> expires,
                                std::unique_ptr<SetCookieCallback> callback) {
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
           &SetCookieOnIO,
          base::Unretained(storage_partition_->GetURLRequestContext()), name,
          value, url.fromMaybe(""), domain.fromMaybe(""), path.fromMaybe(""),
          secure.fromMaybe(false), http_only.fromMaybe(false),
          same_site.fromMaybe(""), expires.fromMaybe(-1),
           base::BindOnce(&CookieSetOnIO, std::move(callback))));
 }
