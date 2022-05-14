GURL URLFixerUpper::FixupRelativeFile(const FilePath& base_dir,
                                      const FilePath& text) {
  FilePath old_cur_directory;
  if (!base_dir.empty()) {
    file_util::GetCurrentDirectory(&old_cur_directory);
    file_util::SetCurrentDirectory(base_dir);
  }

  FilePath::StringType trimmed;
   PrepareStringForFileOps(text, &trimmed);
 
   bool is_file = true;
  // Avoid recognizing definite non-file URLs as file paths.
  GURL gurl(trimmed);
  if (gurl.is_valid() && gurl.IsStandard())
    is_file = false;
   FilePath full_path;
  if (is_file && !ValidPathForFile(trimmed, &full_path)) {
#if defined(OS_WIN)
    std::wstring unescaped = UTF8ToWide(UnescapeURLComponent(
        WideToUTF8(trimmed),
        UnescapeRule::SPACES | UnescapeRule::URL_SPECIAL_CHARS));
#elif defined(OS_POSIX)
    std::string unescaped = UnescapeURLComponent(
        trimmed,
        UnescapeRule::SPACES | UnescapeRule::URL_SPECIAL_CHARS);
#endif

    if (!ValidPathForFile(unescaped, &full_path))
      is_file = false;
  }

  if (!base_dir.empty())
    file_util::SetCurrentDirectory(old_cur_directory);

  if (is_file) {
    GURL file_url = net::FilePathToFileURL(full_path);
    if (file_url.is_valid())
      return GURL(UTF16ToUTF8(net::FormatUrl(file_url, std::string(),
          net::kFormatUrlOmitUsernamePassword, UnescapeRule::NORMAL, NULL,
          NULL, NULL)));
  }

#if defined(OS_WIN)
  std::string text_utf8 = WideToUTF8(text.value());
#elif defined(OS_POSIX)
  std::string text_utf8 = text.value();
#endif
  return FixupURL(text_utf8, std::string());
}
