 void StorageHandler::GetUsageAndQuota(
     const String& origin,
     std::unique_ptr<GetUsageAndQuotaCallback> callback) {
  if (!process_)
     return callback->sendFailure(Response::InternalError());
 
   GURL origin_url(origin);
  if (!origin_url.is_valid()) {
    return callback->sendFailure(
         Response::Error(origin + " is not a valid URL"));
   }
 
  storage::QuotaManager* manager =
      process_->GetStoragePartition()->GetQuotaManager();
   BrowserThread::PostTask(
       BrowserThread::IO, FROM_HERE,
       base::BindOnce(&GetUsageAndQuotaOnIOThread, base::RetainedRef(manager),
                      origin_url, base::Passed(std::move(callback))));
 }
