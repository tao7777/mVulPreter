 bool GetUrlFromHDrop(IDataObject* data_object,
                     base::string16* url,
                      base::string16* title) {
   DCHECK(data_object && url && title);
 
   STGMEDIUM medium;
   if (!GetData(data_object, Clipboard::GetCFHDropFormatType(), &medium))
     return false;
 
  HDROP hdrop = static_cast<HDROP>(GlobalLock(medium.hGlobal));
  if (!hdrop)
    return false;
  bool success = false;
  wchar_t filename[MAX_PATH];
  if (DragQueryFileW(hdrop, 0, filename, arraysize(filename))) {
    wchar_t url_buffer[INTERNET_MAX_URL_LENGTH];
    if (0 == _wcsicmp(PathFindExtensionW(filename), L".url") &&
        GetPrivateProfileStringW(L"InternetShortcut", L"url", 0, url_buffer,
                                 arraysize(url_buffer), filename)) {
      url->assign(url_buffer);
      PathRemoveExtension(filename);
      title->assign(PathFindFileName(filename));
      success = true;
     }
   }
 
  DragFinish(hdrop);
  GlobalUnlock(medium.hGlobal);
   return success;
 }
