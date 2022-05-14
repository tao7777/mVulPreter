bool CopyDirectory(const FilePath& from_path,
                   const FilePath& to_path,
                   bool recursive) {
  base::ThreadRestrictions::AssertIOAllowed();
  DCHECK(to_path.value().find('*') == std::string::npos);
  DCHECK(from_path.value().find('*') == std::string::npos);

  char top_dir[PATH_MAX];
  if (base::strlcpy(top_dir, from_path.value().c_str(),
                    arraysize(top_dir)) >= arraysize(top_dir)) {
    return false;
  }

  FilePath real_to_path = to_path;
  if (PathExists(real_to_path)) {
    if (!AbsolutePath(&real_to_path))
      return false;
  } else {
    real_to_path = real_to_path.DirName();
    if (!AbsolutePath(&real_to_path))
      return false;
  }
  FilePath real_from_path = from_path;
  if (!AbsolutePath(&real_from_path))
    return false;
  if (real_to_path.value().size() >= real_from_path.value().size() &&
      real_to_path.value().compare(0, real_from_path.value().size(),
      real_from_path.value()) == 0)
    return false;

  bool success = true;
  int traverse_type = FileEnumerator::FILES | FileEnumerator::SHOW_SYM_LINKS;
  if (recursive)
    traverse_type |= FileEnumerator::DIRECTORIES;
  FileEnumerator traversal(from_path, recursive, traverse_type);

  FileEnumerator::FindInfo info;
  FilePath current = from_path;
  if (stat(from_path.value().c_str(), &info.stat) < 0) {
    DLOG(ERROR) << "CopyDirectory() couldn't stat source directory: "
                << from_path.value() << " errno = " << errno;
    success = false;
  }
  struct stat to_path_stat;
  FilePath from_path_base = from_path;
  if (recursive && stat(to_path.value().c_str(), &to_path_stat) == 0 &&
      S_ISDIR(to_path_stat.st_mode)) {
    from_path_base = from_path.DirName();
  }

   DCHECK(recursive || S_ISDIR(info.stat.st_mode));
 
   while (success && !current.empty()) {
    std::string suffix(&current.value().c_str()[from_path_base.value().size()]);
    if (!suffix.empty()) {
      DCHECK_EQ('/', suffix[0]);
      suffix.erase(0, 1);
     }
    const FilePath target_path = to_path.Append(suffix);
 
     if (S_ISDIR(info.stat.st_mode)) {
       if (mkdir(target_path.value().c_str(), info.stat.st_mode & 01777) != 0 &&
          errno != EEXIST) {
        DLOG(ERROR) << "CopyDirectory() couldn't create directory: "
                    << target_path.value() << " errno = " << errno;
        success = false;
      }
    } else if (S_ISREG(info.stat.st_mode)) {
      if (!CopyFile(current, target_path)) {
        DLOG(ERROR) << "CopyDirectory() couldn't create file: "
                    << target_path.value();
        success = false;
      }
    } else {
      DLOG(WARNING) << "CopyDirectory() skipping non-regular file: "
                    << current.value();
    }

    current = traversal.Next();
    traversal.GetFindInfo(&info);
  }

  return success;
}
