void GDataCacheMetadataMap::Initialize(
    const std::vector<FilePath>& cache_paths) {
   AssertOnSequencedWorkerPool();
 
   if (cache_paths.size() < GDataCache::NUM_CACHE_TYPES) {
    DLOG(ERROR) << "Size of cache_paths is invalid.";
     return;
   }
 
  if (!CreateCacheDirectories(cache_paths))
     return;
 
  if (!ChangeFilePermissions(cache_paths[GDataCache::CACHE_TYPE_PERSISTENT],
                             S_IRWXU | S_IXGRP | S_IXOTH))
    return;

  DVLOG(1) << "Scanning directories";
 
  ScanCacheDirectory(cache_paths, GDataCache::CACHE_TYPE_PERSISTENT,
                     &cache_map_);
  ScanCacheDirectory(cache_paths, GDataCache::CACHE_TYPE_TMP, &cache_map_);

  // Then scan pinned and outgoing directories to update existing entries in
  // cache map, or create new ones for pinned symlinks to /dev/null which target
  // nothing.
  // Pinned and outgoing directories should be scanned after the persistent
  // directory as we'll add PINNED and DIRTY states respectively to the existing
  // files in the persistent directory per the contents of the pinned and
  // outgoing directories.
  ScanCacheDirectory(cache_paths, GDataCache::CACHE_TYPE_PINNED, &cache_map_);
  ScanCacheDirectory(cache_paths, GDataCache::CACHE_TYPE_OUTGOING, &cache_map_);

   DVLOG(1) << "Directory scan finished";
 }
