DWORD IsReparsePoint(const std::wstring& full_path, bool* result) {
  std::wstring path = full_path;

  if (0 == path.compare(0, kNTPrefixLen, kNTPrefix))
     path = path.substr(kNTPrefixLen);
 
  if (IsPipe(path)) {
     *result = FALSE;
     return ERROR_SUCCESS;
   }

  std::wstring::size_type last_pos = std::wstring::npos;

  do {
    path = path.substr(0, last_pos);

    DWORD attributes = ::GetFileAttributes(path.c_str());
    if (INVALID_FILE_ATTRIBUTES == attributes) {
      DWORD error = ::GetLastError();
      if (error != ERROR_FILE_NOT_FOUND &&
          error != ERROR_PATH_NOT_FOUND &&
          error != ERROR_INVALID_NAME) {
        NOTREACHED();
        return error;
      }
    } else if (FILE_ATTRIBUTE_REPARSE_POINT & attributes) {
      *result = true;
      return ERROR_SUCCESS;
    }

    last_pos = path.rfind(L'\\');
  } while (last_pos != std::wstring::npos);

  *result = false;
   return ERROR_SUCCESS;
 }
