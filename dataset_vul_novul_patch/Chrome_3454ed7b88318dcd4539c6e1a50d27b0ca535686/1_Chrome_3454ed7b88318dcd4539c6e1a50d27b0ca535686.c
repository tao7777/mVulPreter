base::string16 GenerateKeywordFromNavigationEntry(
    const NavigationEntry* entry,
    const std::string& accept_languages) {
  if (IsFormSubmit(entry))
    return base::string16();

  GURL url = entry->GetUserTypedURL();
  if (!url.is_valid()) {
    url = entry->GetURL();
    if (!url.is_valid())
       return base::string16();
   }
 
  if (!url.SchemeIs(url::kHttpScheme) || (url.path().length() > 1))
     return base::string16();
 
   return TemplateURL::GenerateKeyword(url, accept_languages);
 }
