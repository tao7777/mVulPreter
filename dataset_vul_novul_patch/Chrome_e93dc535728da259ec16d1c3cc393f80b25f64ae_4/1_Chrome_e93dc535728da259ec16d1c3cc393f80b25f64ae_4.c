 bool ClipboardUtil::GetHtml(IDataObject* data_object,
                            base::string16* html, std::string* base_url) {
  DCHECK(data_object && html && base_url);

  STGMEDIUM store;
  if (HasData(data_object, Clipboard::GetHtmlFormatType()) &&
       GetData(data_object, Clipboard::GetHtmlFormatType(), &store)) {
     {
      base::win::ScopedHGlobal<char> data(store.hGlobal);
 
       std::string html_utf8;
       CFHtmlToHtml(std::string(data.get(), data.Size()), &html_utf8, base_url);
      html->assign(base::UTF8ToWide(html_utf8));
    }
    ReleaseStgMedium(&store);
    return true;
  }

  if (!HasData(data_object, Clipboard::GetTextHtmlFormatType()))
    return false;

  if (!GetData(data_object, Clipboard::GetTextHtmlFormatType(), &store))
    return false;
 
   {
    base::win::ScopedHGlobal<wchar_t> data(store.hGlobal);
     html->assign(data.get());
   }
   ReleaseStgMedium(&store);
  return true;
}
