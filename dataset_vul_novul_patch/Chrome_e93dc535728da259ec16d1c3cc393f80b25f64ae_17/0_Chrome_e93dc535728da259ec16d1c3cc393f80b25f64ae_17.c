bool OSExchangeDataProviderWin::GetURLAndTitle(
    OSExchangeData::FilenameToURLPolicy policy,
    GURL* url,
    base::string16* title) const {
   base::string16 url_str;
   bool success = ClipboardUtil::GetUrl(
       source_object_,
      url,
       title,
       policy == OSExchangeData::CONVERT_FILENAMES ? true : false);
   if (success) {
    DCHECK(url->is_valid());
    return true;
   } else if (GetPlainTextURL(source_object_, url)) {
     if (url->is_valid())
       *title = net::GetSuggestedFilename(*url, "", "", "", "", std::string());
    else
      title->clear();
    return true;
  }
  return false;
}
