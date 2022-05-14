std::vector<FilePath> GDataCache::GetCachePaths(
    const FilePath& cache_root_path) {
  std::vector<FilePath> cache_paths;
  cache_paths.push_back(cache_root_path.Append(kGDataCacheMetaDir));
  cache_paths.push_back(cache_root_path.Append(kGDataCachePinnedDir));
  cache_paths.push_back(cache_root_path.Append(kGDataCacheOutgoingDir));
  cache_paths.push_back(cache_root_path.Append(kGDataCachePersistentDir));
  cache_paths.push_back(cache_root_path.Append(kGDataCacheTmpDir));
  cache_paths.push_back(cache_root_path.Append(kGDataCacheTmpDownloadsDir));
  cache_paths.push_back(cache_root_path.Append(kGDataCacheTmpDocumentsDir));
  return cache_paths;
}
