bool OSExchangeDataProviderAura::GetURLAndTitle(GURL* url,
                                                base::string16* title) const {
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
