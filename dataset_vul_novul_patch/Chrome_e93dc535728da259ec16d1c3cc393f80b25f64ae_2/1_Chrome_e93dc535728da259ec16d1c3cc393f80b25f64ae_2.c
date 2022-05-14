bool GetFileUrl(IDataObject* data_object, base::string16* url,
                base::string16* title) {
  STGMEDIUM store;
  if (GetData(data_object, Clipboard::GetFilenameWFormatType(), &store)) {
    bool success = false;
    {
      base::win::ScopedHGlobal<wchar_t> data(store.hGlobal);
      if (data.get() && data.get()[0] &&
          (PathFileExists(data.get()) || PathIsUNC(data.get()))) {
        wchar_t file_url[INTERNET_MAX_URL_LENGTH];
        DWORD file_url_len = arraysize(file_url);
        if (SUCCEEDED(::UrlCreateFromPathW(data.get(), file_url, &file_url_len,
                                           0))) {
          url->assign(file_url);
          title->assign(file_url);
          success = true;
        }
      }
    }
    ReleaseStgMedium(&store);
    if (success)
      return true;
  }
  if (GetData(data_object, Clipboard::GetFilenameFormatType(), &store)) {
    bool success = false;
    {
      base::win::ScopedHGlobal<char> data(store.hGlobal);
      if (data.get() && data.get()[0] && (PathFileExistsA(data.get()) ||
                                          PathIsUNCA(data.get()))) {
        char file_url[INTERNET_MAX_URL_LENGTH];
        DWORD file_url_len = arraysize(file_url);
        if (SUCCEEDED(::UrlCreateFromPathA(data.get(), file_url, &file_url_len,
                                           0))) {
          url->assign(base::UTF8ToWide(file_url));
          title->assign(*url);
          success = true;
        }
      }
    }
    ReleaseStgMedium(&store);
    if (success)
      return true;
  }
  return false;
}
