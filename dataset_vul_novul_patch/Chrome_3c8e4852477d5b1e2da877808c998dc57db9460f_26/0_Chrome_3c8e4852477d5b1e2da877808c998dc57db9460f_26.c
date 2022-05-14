Response ServiceWorkerHandler::DispatchSyncEvent(
    const std::string& origin,
    const std::string& registration_id,
    const std::string& tag,
     bool last_chance) {
   if (!enabled_)
     return CreateDomainNotEnabledErrorResponse();
  if (!storage_partition_)
     return CreateContextErrorResponse();
   int64_t id = 0;
   if (!base::StringToInt64(registration_id, &id))
     return CreateInvalidVersionIdErrorResponse();
 
  BackgroundSyncContext* sync_context =
      storage_partition_->GetBackgroundSyncContext();
 
   BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
                           base::BindOnce(&DispatchSyncEventOnIO, context_,
                                         base::WrapRefCounted(sync_context),
                                         GURL(origin), id, tag, last_chance));
  return Response::OK();
}
