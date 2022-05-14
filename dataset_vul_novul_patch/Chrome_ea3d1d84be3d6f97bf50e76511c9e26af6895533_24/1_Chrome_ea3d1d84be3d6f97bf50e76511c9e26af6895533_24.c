 static NPError PostURLNotify(NPP id,
                              const char* url,
                              const char* target,
                             uint32 len,
                             const char* buf,
                             NPBool file,
                             bool notify,
                             void* notify_data) {
  if (!url)
    return NPERR_INVALID_URL;

  scoped_refptr<NPAPI::PluginInstance> plugin = FindInstance(id);
  if (!plugin.get()) {
    NOTREACHED();
    return NPERR_GENERIC_ERROR;
  }

  std::string post_file_contents;

  if (file) {



    if (!buf)
      return NPERR_FILE_NOT_FOUND;

    std::string file_path_ascii(buf);
    std::wstring file_path;
    static const char kFileUrlPrefix[] = "file:";
    if (StartsWithASCII(file_path_ascii, kFileUrlPrefix, false)) {
      GURL file_url(file_path_ascii);
      DCHECK(file_url.SchemeIsFile());
      FilePath path;
      net::FileURLToFilePath(file_url, &path);
      file_path = path.ToWStringHack();
    } else {
      file_path = base::SysNativeMBToWide(file_path_ascii);
    }

    file_util::FileInfo post_file_info = {0};
    if (!file_util::GetFileInfo(file_path.c_str(), &post_file_info) ||
        post_file_info.is_directory)
      return NPERR_FILE_NOT_FOUND;

    if (!file_util::ReadFileToString(file_path, &post_file_contents))
      return NPERR_FILE_NOT_FOUND;

    buf = post_file_contents.c_str();
     len = post_file_contents.size();
   }
 
  bool is_javascript_url = IsJavaScriptUrl(url);
  plugin->webplugin()->HandleURLRequest(
      "POST", is_javascript_url, target, len, buf, false, notify, url,
      reinterpret_cast<intptr_t>(notify_data), plugin->popups_allowed());
   return NPERR_NO_ERROR;
 }
