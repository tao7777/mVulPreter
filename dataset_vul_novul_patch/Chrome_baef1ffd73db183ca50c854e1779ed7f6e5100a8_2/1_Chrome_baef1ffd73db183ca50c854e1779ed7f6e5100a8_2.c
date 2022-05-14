void GDataCache::Pin(const std::string& resource_id,
                     const std::string& md5,
                     FileOperationType file_operation_type,
                     base::PlatformFileError* error) {
  AssertOnSequencedWorkerPool();
  DCHECK(error);

  FilePath source_path;
  FilePath dest_path;
  FilePath symlink_path;
  bool create_symlink = true;
  int cache_state = CACHE_STATE_PINNED;
  CacheSubDirectoryType sub_dir_type = CACHE_TYPE_PERSISTENT;

  scoped_ptr<CacheEntry> cache_entry = GetCacheEntry(resource_id, md5);

  if (!cache_entry.get()) {  // Entry does not exist in cache.
    dest_path = FilePath::FromUTF8Unsafe(util::kSymLinkToDevNull);
     source_path = dest_path;
 
    sub_dir_type = CACHE_TYPE_PINNED;
  } else {  // File exists in cache, determines destination path.
    cache_state |= cache_entry->cache_state;


    if (cache_entry->IsDirty() || cache_entry->IsMounted()) {
      DCHECK_EQ(CACHE_TYPE_PERSISTENT, cache_entry->sub_dir_type);
      dest_path = GetCacheFilePath(resource_id,
                                   md5,
                                   cache_entry->sub_dir_type,
                                   CACHED_FILE_LOCALLY_MODIFIED);
      source_path = dest_path;
    } else {
      source_path = GetCacheFilePath(resource_id,
                                     md5,
                                     cache_entry->sub_dir_type,
                                     CACHED_FILE_FROM_SERVER);

      if (cache_entry->sub_dir_type == CACHE_TYPE_PINNED) {
        dest_path = source_path;
        create_symlink = false;
      } else {  // File exists, move it to persistent dir.
        dest_path = GetCacheFilePath(resource_id,
                                     md5,
                                     CACHE_TYPE_PERSISTENT,
                                     CACHED_FILE_FROM_SERVER);
      }
    }
  }

  if (create_symlink) {
    symlink_path = GetCacheFilePath(resource_id,
                                    std::string(),
                                    CACHE_TYPE_PINNED,
                                    CACHED_FILE_FROM_SERVER);
  }

  *error = ModifyCacheState(source_path,
                            dest_path,
                            file_operation_type,
                            symlink_path,
                            create_symlink);

  if (*error == base::PLATFORM_FILE_OK) {
    metadata_->UpdateCache(resource_id, md5, sub_dir_type, cache_state);
  }
}
