void RemoveInvalidFilesFromPersistentDirectory(
    const GDataCacheMetadataMap::ResourceIdToFilePathMap& persistent_file_map,
    const GDataCacheMetadataMap::ResourceIdToFilePathMap& outgoing_file_map,
    GDataCacheMetadataMap::CacheMap* cache_map) {
  for (GDataCacheMetadataMap::ResourceIdToFilePathMap::const_iterator iter =
           persistent_file_map.begin();
       iter != persistent_file_map.end(); ++iter) {
    const std::string& resource_id = iter->first;
    const FilePath& file_path = iter->second;
    GDataCacheMetadataMap::CacheMap::iterator cache_map_iter =
        cache_map->find(resource_id);
    if (cache_map_iter != cache_map->end()) {
      const GDataCache::CacheEntry& cache_entry = cache_map_iter->second;
      if (cache_entry.IsDirty() && outgoing_file_map.count(resource_id) == 0) {
        LOG(WARNING) << "Removing dirty-but-not-committed file: "
                     << file_path.value();
        file_util::Delete(file_path, false);
        cache_map->erase(cache_map_iter);
      }
      if (!cache_entry.IsDirty() && !cache_entry.IsPinned()) {
        LOG(WARNING) << "Removing persistent-but-dangling file: "
                     << file_path.value();
        file_util::Delete(file_path, false);
        cache_map->erase(cache_map_iter);
      }
    }
  }
}
