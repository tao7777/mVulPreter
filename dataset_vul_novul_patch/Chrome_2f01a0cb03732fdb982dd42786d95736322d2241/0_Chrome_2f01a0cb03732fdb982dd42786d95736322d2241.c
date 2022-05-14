bool SetExtendedFileAttribute(const char* path,
                              const char* name,
                               const char* value,
                               size_t value_size,
                               int flags) {
//// On Chrome OS, there is no component that can validate these extended
//// attributes so there is no need to set them.
#if !defined(OS_CHROMEOS)
   base::ScopedBlockingCall scoped_blocking_call(base::BlockingType::MAY_BLOCK);
   int result = setxattr(path, name, value, value_size, flags);
   if (result) {
     DPLOG(ERROR) << "Could not set extended attribute " << name << " on file "
                  << path;
     return false;
   }
#endif  // !defined(OS_CHROMEOS)
   return true;
 }
