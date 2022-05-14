bool OSExchangeData::GetURLAndTitle(GURL* url, base::string16* title) const {
bool OSExchangeData::GetURLAndTitle(FilenameToURLPolicy policy,
                                    GURL* url,
                                    base::string16* title) const {
  return provider_->GetURLAndTitle(policy, url, title);
 }
