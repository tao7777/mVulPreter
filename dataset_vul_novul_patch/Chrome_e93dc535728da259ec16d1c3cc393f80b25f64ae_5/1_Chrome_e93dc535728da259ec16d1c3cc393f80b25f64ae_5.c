 bool ClipboardUtil::GetPlainText(IDataObject* data_object,
                                 base::string16* plain_text) {
  DCHECK(data_object && plain_text);
  if (!HasPlainText(data_object))
    return false;

  STGMEDIUM store;
   if (GetData(data_object, Clipboard::GetPlainTextWFormatType(), &store)) {
     {
      base::win::ScopedHGlobal<wchar_t> data(store.hGlobal);
       plain_text->assign(data.get());
     }
     ReleaseStgMedium(&store);
    return true;
  }

   if (GetData(data_object, Clipboard::GetPlainTextFormatType(), &store)) {
     {
      base::win::ScopedHGlobal<char> data(store.hGlobal);
       plain_text->assign(base::UTF8ToWide(data.get()));
     }
     ReleaseStgMedium(&store);
    return true;
  }
 
   base::string16 title;
  return GetUrl(data_object, plain_text, &title, false);
 }
