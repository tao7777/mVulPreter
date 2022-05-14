 bool GetUrlFromHDrop(IDataObject* data_object,
                     GURL* url,
                      base::string16* title) {
   DCHECK(data_object && url && title);
 
  bool success = false;
   STGMEDIUM medium;
   if (!GetData(data_object, Clipboard::GetCFHDropFormatType(), &medium))
     return false;
 
  {
    base::win::ScopedHGlobal<HDROP> hdrop(medium.hGlobal);

    if (!hdrop.get())
      return false;

    wchar_t filename[MAX_PATH];
    if (DragQueryFileW(hdrop.get(), 0, filename, arraysize(filename))) {
      wchar_t url_buffer[INTERNET_MAX_URL_LENGTH];
      if (0 == _wcsicmp(PathFindExtensionW(filename), L".url") &&
          GetPrivateProfileStringW(L"InternetShortcut",
                                   L"url",
                                   0,
                                   url_buffer,
                                   arraysize(url_buffer),
                                   filename)) {
        *url = GURL(url_buffer);
        PathRemoveExtension(filename);
        title->assign(PathFindFileName(filename));
        success = url->is_valid();
      }
     }
   }
 
  ReleaseStgMedium(&medium);
   return success;
 }
