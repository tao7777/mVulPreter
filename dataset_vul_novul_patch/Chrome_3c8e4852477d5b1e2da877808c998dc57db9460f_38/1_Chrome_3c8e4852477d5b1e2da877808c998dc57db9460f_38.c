 Response StorageHandler::UntrackIndexedDBForOrigin(const std::string& origin) {
  if (!process_)
     return Response::InternalError();
 
   GURL origin_url(origin);
  if (!origin_url.is_valid())
    return Response::InvalidParams(origin + " is not a valid URL");

  GetIndexedDBObserver()->TaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&IndexedDBObserver::UntrackOriginOnIDBThread,
                                base::Unretained(GetIndexedDBObserver()),
                                url::Origin::Create(origin_url)));
  return Response::OK();
}
