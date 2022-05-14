bool IsValidSymbolicLink(const FilePath& file_path,
                         GDataCache::CacheSubDirectoryType sub_dir_type,
                         const std::vector<FilePath>& cache_paths,
                         std::string* reason) {
  DCHECK(sub_dir_type == GDataCache::CACHE_TYPE_PINNED ||
         sub_dir_type == GDataCache::CACHE_TYPE_OUTGOING);
  FilePath destination;
  if (!file_util::ReadSymbolicLink(file_path, &destination)) {
    *reason = "failed to read the symlink (maybe not a symlink)";
    return false;
  }
  if (!file_util::PathExists(destination)) {
    *reason = "pointing to a non-existent file";
    return false;
  }
  if (sub_dir_type == GDataCache::CACHE_TYPE_PINNED &&
      destination == FilePath::FromUTF8Unsafe(util::kSymLinkToDevNull)) {
    return true;
  }
  if (!cache_paths[GDataCache::CACHE_TYPE_PERSISTENT].IsParent(destination)) {
    *reason = "pointing to a file outside of persistent directory";
    return false;
  }
  return true;
}
