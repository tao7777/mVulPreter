void GDataCacheMetadataMap::Initialize(
    const std::vector<FilePath>& cache_paths) {
   AssertOnSequencedWorkerPool();
 
   if (cache_paths.size() < GDataCache::NUM_CACHE_TYPES) {
    LOG(ERROR) << "Size of cache_paths is invalid.";
     return;
   }
 
  if (!GDataCache::CreateCacheDirectories(cache_paths))
     return;
 
  if (!ChangeFilePermissions(cache_paths[GDataCache::CACHE_TYPE_PERSISTENT],
                             S_IRWXU | S_IXGRP | S_IXOTH))
    return;

  DVLOG(1) << "Scanning directories";
 
  ResourceIdToFilePathMap persistent_file_map;
  ScanCacheDirectory(cache_paths,
                     GDataCache::CACHE_TYPE_PERSISTENT,
                     &cache_map_,
                     &persistent_file_map);
  ResourceIdToFilePathMap tmp_file_map;
  ScanCacheDirectory(cache_paths,
                     GDataCache::CACHE_TYPE_TMP,
                     &cache_map_,
                     &tmp_file_map);
  ResourceIdToFilePathMap pinned_file_map;
  ScanCacheDirectory(cache_paths,
                     GDataCache::CACHE_TYPE_PINNED,
                     &cache_map_,
                     &pinned_file_map);
  ResourceIdToFilePathMap outgoing_file_map;
  ScanCacheDirectory(cache_paths,
                     GDataCache::CACHE_TYPE_OUTGOING,
                     &cache_map_,
                     &outgoing_file_map);
  RemoveInvalidFilesFromPersistentDirectory(persistent_file_map,
                                            outgoing_file_map,
                                            &cache_map_);
   DVLOG(1) << "Directory scan finished";
 }
