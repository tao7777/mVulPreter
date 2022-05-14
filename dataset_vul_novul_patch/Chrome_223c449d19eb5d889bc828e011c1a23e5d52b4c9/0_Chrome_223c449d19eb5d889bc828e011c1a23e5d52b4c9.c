FilePath GetSuggestedFilename(const GURL& url,
                              const std::string& content_disposition,
                              const std::string& referrer_charset,
                              const FilePath& default_name) {
  static const FilePath::CharType kFinalFallbackName[] =
      FILE_PATH_LITERAL("download");

  if (url.SchemeIs("about") || url.SchemeIs("data")) {
    return default_name.empty() ? FilePath(kFinalFallbackName) : default_name;
  }

  const std::string filename_from_cd = GetFileNameFromCD(content_disposition,
                                                          referrer_charset);
#if defined(OS_WIN)
  FilePath::StringType filename = UTF8ToWide(filename_from_cd);
#elif defined(OS_POSIX)
  FilePath::StringType filename = filename_from_cd;
#endif

  if (!filename.empty()) {
    filename = FilePath(filename).BaseName().value();

    TrimString(filename, FILE_PATH_LITERAL("."), &filename);
  }
  if (filename.empty()) {
    if (url.is_valid()) {
      const std::string unescaped_url_filename = UnescapeURLComponent(
          url.ExtractFileName(),
          UnescapeRule::SPACES | UnescapeRule::URL_SPECIAL_CHARS);
#if defined(OS_WIN)
      filename = UTF8ToWide(unescaped_url_filename);
#elif defined(OS_POSIX)
      filename = unescaped_url_filename;
#endif
     }
   }
 
#if defined(OS_WIN)
  { // Handle CreateFile() stripping trailing dots and spaces on filenames
    // http://support.microsoft.com/kb/115827
    std::string::size_type pos = filename.find_last_not_of(L" .");
    if (pos == std::string::npos)
      filename.resize(0);
    else
      filename.resize(++pos);
  }
#endif
   TrimString(filename, FILE_PATH_LITERAL("."), &filename);
 
  if (filename.empty()) {
    if (!default_name.empty()) {
      filename = default_name.value();
    } else if (url.is_valid()) {
      filename = url.host().empty() ? kFinalFallbackName :
#if defined(OS_WIN)
          UTF8ToWide(url.host());
#elif defined(OS_POSIX)
          url.host();
#endif
    } else {
      NOTREACHED();
    }
  }

  file_util::ReplaceIllegalCharactersInPath(&filename, '-');
  return FilePath(filename);
}
