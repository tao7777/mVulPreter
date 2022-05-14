bool OSExchangeData::GetURLAndTitle(GURL* url, base::string16* title) const {
  return provider_->GetURLAndTitle(url, title);
 }
