base::PlatformFileError ModifyCacheState(
    const FilePath& source_path,
    const FilePath& dest_path,
    GDataCache::FileOperationType file_operation_type,
    const FilePath& symlink_path,
    bool create_symlink) {
  if (source_path != dest_path) {
    bool success = false;
    if (file_operation_type == GDataCache::FILE_OPERATION_MOVE)
      success = file_util::Move(source_path, dest_path);
    else if (file_operation_type == GDataCache::FILE_OPERATION_COPY)
      success = file_util::CopyFile(source_path, dest_path);
    if (!success) {
      base::PlatformFileError error = SystemToPlatformError(errno);
      PLOG(ERROR) << "Error "
                  << (file_operation_type == GDataCache::FILE_OPERATION_MOVE ?
                      "moving " : "copying ")
                  << source_path.value()
                  << " to " << dest_path.value();
      return error;
    } else {
      DVLOG(1) << (file_operation_type == GDataCache::FILE_OPERATION_MOVE ?
                   "Moved " : "Copied ")
               << source_path.value()
               << " to " << dest_path.value();
    }
  } else {
    DVLOG(1) << "No need to move file: source = destination";
  }

  if (symlink_path.empty())
    return base::PLATFORM_FILE_OK;
 
  bool deleted = util::DeleteSymlink(symlink_path);
   if (deleted) {
     DVLOG(1) << "Deleted symlink " << symlink_path.value();
   } else {
    if (errno != ENOENT)
      PLOG(WARNING) << "Error deleting symlink " << symlink_path.value();
  }

  if (!create_symlink)
    return base::PLATFORM_FILE_OK;

  if (!file_util::CreateSymbolicLink(dest_path, symlink_path)) {
    base::PlatformFileError error = SystemToPlatformError(errno);
    PLOG(ERROR) << "Error creating symlink " << symlink_path.value()
                << " for " << dest_path.value();
    return error;
  } else {
    DVLOG(1) << "Created symlink " << symlink_path.value()
             << " to " << dest_path.value();
  }

  return base::PLATFORM_FILE_OK;
}
