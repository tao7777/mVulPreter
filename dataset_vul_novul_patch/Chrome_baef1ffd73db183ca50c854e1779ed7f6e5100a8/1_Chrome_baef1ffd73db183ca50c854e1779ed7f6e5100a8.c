bool GDataCache::CreateCacheDirectories(
    const std::vector<FilePath>& paths_to_create) {
  bool success = true;
  for (size_t i = 0; i < paths_to_create.size(); ++i) {
    if (file_util::DirectoryExists(paths_to_create[i]))
      continue;
    if (!file_util::CreateDirectory(paths_to_create[i])) {
      success = false;
      PLOG(ERROR) << "Error creating directory " << paths_to_create[i].value();
    } else {
      DVLOG(1) << "Created directory " << paths_to_create[i].value();
    }
  }
  return success;
}
