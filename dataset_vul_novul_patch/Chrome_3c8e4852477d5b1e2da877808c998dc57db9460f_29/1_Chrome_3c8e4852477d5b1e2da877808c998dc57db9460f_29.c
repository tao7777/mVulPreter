void StorageHandler::ClearDataForOrigin(
     const std::string& origin,
     const std::string& storage_types,
     std::unique_ptr<ClearDataForOriginCallback> callback) {
  if (!process_)
     return callback->sendFailure(Response::InternalError());
 
  StoragePartition* partition = process_->GetStoragePartition();
   std::vector<std::string> types = base::SplitString(
       storage_types, ",", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
   std::unordered_set<std::string> set(types.begin(), types.end());
  uint32_t remove_mask = 0;
  if (set.count(Storage::StorageTypeEnum::Appcache))
    remove_mask |= StoragePartition::REMOVE_DATA_MASK_APPCACHE;
  if (set.count(Storage::StorageTypeEnum::Cookies))
    remove_mask |= StoragePartition::REMOVE_DATA_MASK_COOKIES;
  if (set.count(Storage::StorageTypeEnum::File_systems))
    remove_mask |= StoragePartition::REMOVE_DATA_MASK_FILE_SYSTEMS;
  if (set.count(Storage::StorageTypeEnum::Indexeddb))
    remove_mask |= StoragePartition::REMOVE_DATA_MASK_INDEXEDDB;
  if (set.count(Storage::StorageTypeEnum::Local_storage))
    remove_mask |= StoragePartition::REMOVE_DATA_MASK_LOCAL_STORAGE;
  if (set.count(Storage::StorageTypeEnum::Shader_cache))
    remove_mask |= StoragePartition::REMOVE_DATA_MASK_SHADER_CACHE;
  if (set.count(Storage::StorageTypeEnum::Websql))
    remove_mask |= StoragePartition::REMOVE_DATA_MASK_WEBSQL;
  if (set.count(Storage::StorageTypeEnum::Service_workers))
    remove_mask |= StoragePartition::REMOVE_DATA_MASK_SERVICE_WORKERS;
  if (set.count(Storage::StorageTypeEnum::Cache_storage))
    remove_mask |= StoragePartition::REMOVE_DATA_MASK_CACHE_STORAGE;
  if (set.count(Storage::StorageTypeEnum::All))
    remove_mask |= StoragePartition::REMOVE_DATA_MASK_ALL;

  if (!remove_mask) {
    return callback->sendFailure(
         Response::InvalidParams("No valid storage type specified"));
   }
 
  partition->ClearData(remove_mask,
                       StoragePartition::QUOTA_MANAGED_STORAGE_MASK_ALL,
                       GURL(origin), StoragePartition::OriginMatcherFunction(),
                       base::Time(), base::Time::Max(),
                       base::BindOnce(&ClearDataForOriginCallback::sendSuccess,
                                      std::move(callback)));
 }
