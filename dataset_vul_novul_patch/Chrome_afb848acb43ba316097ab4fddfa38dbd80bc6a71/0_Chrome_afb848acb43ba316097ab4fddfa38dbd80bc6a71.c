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
 
    // Make sure that the file is opened without any permission
    // to other users on the system.
    const mode_t kOwnerOnly = S_IRUSR | S_IWUSR;

    // First, try to create the file.
    int fd = HANDLE_EINTR(
        open(path.value().c_str(), O_RDWR | O_CREAT | O_EXCL, kOwnerOnly));
    if (fd == -1 && options.open_existing) {
      // If this doesn't work, try and open an existing file in append mode.
      // Opening an existing file in a world writable directory has two main
      // security implications:
      // - Attackers could plant a file under their control, so ownership of
      //   the file is checked below.
      // - Attackers could plant a symbolic link so that an unexpected file
      //   is opened, so O_NOFOLLOW is passed to open().
      fd = HANDLE_EINTR(
          open(path.value().c_str(), O_RDWR | O_APPEND | O_NOFOLLOW));

      // Check that the current user owns the file.
      // If uid != euid, then a more complex permission model is used and this
      // API is not appropriate.
      const uid_t real_uid = getuid();
      const uid_t effective_uid = geteuid();
      struct stat sb;
      if (fd >= 0 &&
          (fstat(fd, &sb) != 0 || sb.st_uid != real_uid ||
           sb.st_uid != effective_uid)) {
        LOG(ERROR) <<
            "Invalid owner when opening existing shared memory file.";
        HANDLE_EINTR(close(fd));
        return false;
      }

      // An existing file was opened, so its size should not be fixed.
       fix_size = false;
     }
    fp = NULL;
    if (fd >= 0) {
      // "a+" is always appropriate: if it's a new file, a+ is similar to w+.
      fp = fdopen(fd, "a+");
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
