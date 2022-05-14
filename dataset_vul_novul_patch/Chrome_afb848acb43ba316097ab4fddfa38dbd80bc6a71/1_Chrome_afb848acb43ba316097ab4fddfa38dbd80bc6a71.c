bool SharedMemory::Create(const SharedMemoryCreateOptions& options) {
  DCHECK_EQ(-1, mapped_file_);
  if (options.size == 0) return false;

  if (options.size > static_cast<size_t>(std::numeric_limits<int>::max()))
    return false;

  base::ThreadRestrictions::ScopedAllowIO allow_io;

  FILE *fp;
  bool fix_size = true;

  FilePath path;
  if (options.name == NULL || options.name->empty()) {
    DCHECK(!options.open_existing);
    fp = file_util::CreateAndOpenTemporaryShmemFile(&path, options.executable);

    if (fp) {
      if (unlink(path.value().c_str()))
        PLOG(WARNING) << "unlink";
    }
  } else {
     if (!FilePathForMemoryName(*options.name, &path))
       return false;
 
    fp = file_util::OpenFile(path, "w+x");
    if (fp == NULL && options.open_existing) {
      fp = file_util::OpenFile(path, "a+");
       fix_size = false;
     }
   }
   if (fp && fix_size) {
    struct stat stat;
    if (fstat(fileno(fp), &stat) != 0) {
      file_util::CloseFile(fp);
      return false;
    }
    const size_t current_size = stat.st_size;
    if (current_size != options.size) {
      if (HANDLE_EINTR(ftruncate(fileno(fp), options.size)) != 0) {
        file_util::CloseFile(fp);
        return false;
      }
    }
    requested_size_ = options.size;
  }
  if (fp == NULL) {
#if !defined(OS_MACOSX)
    PLOG(ERROR) << "Creating shared memory in " << path.value() << " failed";
    FilePath dir = path.DirName();
    if (access(dir.value().c_str(), W_OK | X_OK) < 0) {
      PLOG(ERROR) << "Unable to access(W_OK|X_OK) " << dir.value();
      if (dir.value() == "/dev/shm") {
        LOG(FATAL) << "This is frequently caused by incorrect permissions on "
                   << "/dev/shm.  Try 'sudo chmod 1777 /dev/shm' to fix.";
      }
    }
#else
    PLOG(ERROR) << "Creating shared memory in " << path.value() << " failed";
#endif
    return false;
  }

  return PrepareMapFile(fp);
}
