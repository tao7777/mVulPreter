 bool ClipboardUtil::GetUrl(IDataObject* data_object,
    base::string16* url, base::string16* title, bool convert_filenames) {
   DCHECK(data_object && url && title);
   if (!HasUrl(data_object, convert_filenames))
     return false;

  STGMEDIUM store;
  if (GetUrlFromHDrop(data_object, url, title))
    return true;

  if (GetData(data_object, Clipboard::GetMozUrlFormatType(), &store) ||
       GetData(data_object, Clipboard::GetUrlWFormatType(), &store)) {
     {
      base::win::ScopedHGlobal<wchar_t> data(store.hGlobal);
       SplitUrlAndTitle(data.get(), url, title);
     }
     ReleaseStgMedium(&store);
    return true;
   }
 
   if (GetData(data_object, Clipboard::GetUrlFormatType(), &store)) {
     {
      base::win::ScopedHGlobal<char> data(store.hGlobal);
       SplitUrlAndTitle(base::UTF8ToWide(data.get()), url, title);
     }
     ReleaseStgMedium(&store);
    return true;
   }
 
   if (convert_filenames) {
    return GetFileUrl(data_object, url, title);
  } else {
    return false;
   }
 }
