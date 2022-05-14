 void GDataCacheMetadataMap::ScanCacheDirectory(
     const std::vector<FilePath>& cache_paths,
     GDataCache::CacheSubDirectoryType sub_dir_type,
    CacheMap* cache_map,
    ResourceIdToFilePathMap* processed_file_map) {
  DCHECK(cache_map);
  DCHECK(processed_file_map);
   file_util::FileEnumerator enumerator(
       cache_paths[sub_dir_type],
       false,  // not recursive
      static_cast<file_util::FileEnumerator::FileType>(
          file_util::FileEnumerator::FILES |
          file_util::FileEnumerator::SHOW_SYM_LINKS),
      util::kWildCard);
  for (FilePath current = enumerator.Next(); !current.empty();
       current = enumerator.Next()) {
    std::string resource_id;
    std::string md5;
    std::string extra_extension;
    util::ParseCacheFilePath(current, &resource_id, &md5, &extra_extension);

    int cache_state = GDataCache::CACHE_STATE_NONE;
     if (sub_dir_type == GDataCache::CACHE_TYPE_PINNED) {
      std::string reason;
      if (!IsValidSymbolicLink(current, sub_dir_type, cache_paths, &reason)) {
        LOG(WARNING) << "Removing an invalid symlink: " << current.value()
                     << ": " << reason;
        util::DeleteSymlink(current);
        continue;
      }
       CacheMap::iterator iter = cache_map->find(resource_id);
       if (iter != cache_map->end()) {  // Entry exists, update pinned state.
         iter->second.cache_state =
             GDataCache::SetCachePinned(iter->second.cache_state);
        processed_file_map->insert(std::make_pair(resource_id, current));
         continue;
       }
       cache_state = GDataCache::SetCachePinned(cache_state);
     } else if (sub_dir_type == GDataCache::CACHE_TYPE_OUTGOING) {
      std::string reason;
      if (!IsValidSymbolicLink(current, sub_dir_type, cache_paths, &reason)) {
        LOG(WARNING) << "Removing an invalid symlink: " << current.value()
                     << ": " << reason;
        util::DeleteSymlink(current);
        continue;
      }
       CacheMap::iterator iter = cache_map->find(resource_id);
      if (iter == cache_map->end() || !iter->second.IsDirty()) {
        LOG(WARNING) << "Removing an symlink to a non-dirty file: "
                     << current.value();
        util::DeleteSymlink(current);
        continue;
      }
      processed_file_map->insert(std::make_pair(resource_id, current));
      continue;
    } else if (sub_dir_type == GDataCache::CACHE_TYPE_PERSISTENT ||
               sub_dir_type == GDataCache::CACHE_TYPE_TMP) {
      FilePath unused;
      if (file_util::ReadSymbolicLink(current, &unused)) {
        LOG(WARNING) << "Removing a symlink in persistent/tmp directory"
                     << current.value();
        util::DeleteSymlink(current);
        continue;
      }
      if (extra_extension == util::kMountedArchiveFileExtension) {
        DCHECK(sub_dir_type == GDataCache::CACHE_TYPE_PERSISTENT);
        file_util::Delete(current, false);
       } else {
        cache_state = GDataCache::SetCachePresent(cache_state);
        if (md5 == util::kLocallyModifiedFileExtension) {
          if (sub_dir_type == GDataCache::CACHE_TYPE_PERSISTENT) {
            cache_state |= GDataCache::SetCacheDirty(cache_state);
          } else {
            LOG(WARNING) << "Removing a dirty file in tmp directory: "
                         << current.value();
            file_util::Delete(current, false);
            continue;
          }
        }
       }
     } else {
      NOTREACHED() << "Unexpected sub directory type: " << sub_dir_type;
     }
 
     cache_map->insert(std::make_pair(
         resource_id, GDataCache::CacheEntry(md5, sub_dir_type, cache_state)));
    processed_file_map->insert(std::make_pair(resource_id, current));
   }
 }
