bool OSExchangeDataProviderWin::GetURLAndTitle(
    OSExchangeData::FilenameToURLPolicy policy,
    GURL* url,
    base::string16* title) const {
   base::string16 url_str;
   bool success = ClipboardUtil::GetUrl(
       source_object_,
      &url_str,
       title,
       policy == OSExchangeData::CONVERT_FILENAMES ? true : false);
   if (success) {
    GURL test_url(url_str);
    if (test_url.is_valid()) {
      *url = test_url;
      return true;
    }
   } else if (GetPlainTextURL(source_object_, url)) {
     if (url->is_valid())
       *title = net::GetSuggestedFilename(*url, "", "", "", "", std::string());
    else
      title->clear();
    return true;
  }
  return false;
}
