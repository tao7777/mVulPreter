 void GDataCacheMetadataMap::ScanCacheDirectory(
     const std::vector<FilePath>& cache_paths,
     GDataCache::CacheSubDirectoryType sub_dir_type,
    CacheMap* cache_map) {
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
       CacheMap::iterator iter = cache_map->find(resource_id);
       if (iter != cache_map->end()) {  // Entry exists, update pinned state.
         iter->second.cache_state =
             GDataCache::SetCachePinned(iter->second.cache_state);
         continue;
       }
       cache_state = GDataCache::SetCachePinned(cache_state);
     } else if (sub_dir_type == GDataCache::CACHE_TYPE_OUTGOING) {
      // If we're scanning outgoing directory, entry must exist, update its
      // dirty state.
      // If entry doesn't exist, it's a logic error from previous execution,
      // ignore this outgoing symlink and move on.
       CacheMap::iterator iter = cache_map->find(resource_id);
      if (iter != cache_map->end()) {  // Entry exists, update dirty state.
        iter->second.cache_state =
            GDataCache::SetCacheDirty(iter->second.cache_state);
       } else {
        NOTREACHED() << "Dirty cache file MUST have actual file blob";
       }
      continue;
    } else if (extra_extension == util::kMountedArchiveFileExtension) {
      // Mounted archives in cache should be unmounted upon logout/shutdown.
      // But if we encounter a mounted file at start, delete it and create an
      // entry with not PRESENT state.
      DCHECK(sub_dir_type == GDataCache::CACHE_TYPE_PERSISTENT);
      file_util::Delete(current, false);
     } else {
      // Scanning other directories means that cache file is actually present.
      cache_state = GDataCache::SetCachePresent(cache_state);
     }
 
     cache_map->insert(std::make_pair(
         resource_id, GDataCache::CacheEntry(md5, sub_dir_type, cache_state)));
   }
 }
