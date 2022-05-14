bool ReturnsValidPath(int dir_type) {
  base::FilePath path;
  bool result = PathService::Get(dir_type, &path);

  bool check_path_exists = true;
#if defined(OS_POSIX)
  if (dir_type == base::DIR_CACHE)
    check_path_exists = false;
#endif
#if defined(OS_LINUX)
  if (dir_type == base::DIR_USER_DESKTOP)
    check_path_exists = false;
#endif
#if defined(OS_WIN)
  if (dir_type == base::DIR_DEFAULT_USER_QUICK_LAUNCH) {
    if (base::win::GetVersion() < base::win::VERSION_VISTA) {
      wchar_t default_profile_path[MAX_PATH];
      DWORD size = arraysize(default_profile_path);
      return (result &&
              ::GetDefaultUserProfileDirectory(default_profile_path, &size) &&
              StartsWith(path.value(), default_profile_path, false));
    }
  } else if (dir_type == base::DIR_TASKBAR_PINS) {
    if (base::win::GetVersion() < base::win::VERSION_WIN7)
       check_path_exists = false;
   }
 #endif
#if defined(OS_MACOSX)
   if (dir_type != base::DIR_EXE && dir_type != base::DIR_MODULE &&
       dir_type != base::FILE_EXE && dir_type != base::FILE_MODULE) {
     if (path.ReferencesParent())
      return false;
  }
#else
  if (path.ReferencesParent())
    return false;
#endif
  return result && !path.empty() && (!check_path_exists ||
                                     file_util::PathExists(path));
}
