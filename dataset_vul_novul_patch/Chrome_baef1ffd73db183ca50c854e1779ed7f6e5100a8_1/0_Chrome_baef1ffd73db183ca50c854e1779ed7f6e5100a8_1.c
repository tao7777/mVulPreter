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
 

  // Cannot use file_util::Delete which uses stat64 to check if path exists
  // before deleting it.  If path is a symlink, stat64 dereferences it to the
  // target file, so it's in essence checking if the target file exists.
  // Here in this function, if |symlink_path| references |source_path| and
  // |source_path| has just been moved to |dest_path| (e.g. during unpinning),
  // symlink will dereference to a non-existent file.  This results in stat64
  // failing and file_util::Delete bailing out without deleting the symlink.
  // We clearly want the symlink deleted even if it dereferences to nothing.
  // Unfortunately, deleting the symlink before moving the files won't work for
  // the case where move operation fails, but the symlink has already been
  // deleted, which shouldn't happen.  An example scenario is where an existing
  // file is stored to cache and pinned for a specific resource id and md5, then
  // a non-existent file is stored to cache for the same resource id and md5.
  // The 2nd store-to-cache operation fails when moving files, but the symlink
  // created by previous pin operation has already been deleted.
  // We definitely want to keep the pinned state of the symlink if subsequent
  // operations fail.
  // This problem is filed at http://crbug.com/119430.

  bool deleted = HANDLE_EINTR(unlink(symlink_path.value().c_str())) == 0;
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
