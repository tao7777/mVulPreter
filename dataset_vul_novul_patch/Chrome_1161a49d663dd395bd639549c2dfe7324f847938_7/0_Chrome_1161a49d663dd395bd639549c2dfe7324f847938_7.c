bool OSExchangeDataProviderAura::GetURLAndTitle(GURL* url,
bool OSExchangeDataProviderAura::GetURLAndTitle(
    OSExchangeData::FilenameToURLPolicy policy,
    GURL* url,
    base::string16* title) const {
  // TODO(dcheng): implement filename conversion.
   if ((formats_ & OSExchangeData::URL) == 0) {
     title->clear();
     return GetPlainTextURL(url);
  }

  if (!url_.is_valid())
    return false;

  *url = url_;
  *title = title_;
  return true;
}
