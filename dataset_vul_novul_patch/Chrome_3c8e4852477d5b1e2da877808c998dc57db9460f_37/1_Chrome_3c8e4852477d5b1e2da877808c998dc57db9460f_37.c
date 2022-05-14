 Response StorageHandler::UntrackCacheStorageForOrigin(
     const std::string& origin) {
  if (!process_)
     return Response::InternalError();
 
   GURL origin_url(origin);
  if (!origin_url.is_valid())
    return Response::InvalidParams(origin + " is not a valid URL");

  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::BindOnce(&CacheStorageObserver::UntrackOriginOnIOThread,
                     base::Unretained(GetCacheStorageObserver()),
                     url::Origin::Create(origin_url)));
  return Response::OK();
 }
