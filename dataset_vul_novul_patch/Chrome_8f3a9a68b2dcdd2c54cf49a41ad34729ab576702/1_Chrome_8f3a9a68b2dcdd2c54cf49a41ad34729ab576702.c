 bool Browser::ShouldFocusLocationBarByDefault(WebContents* source) {
   const content::NavigationEntry* entry =
       source->GetController().GetActiveEntry();
   if (entry) {
    const GURL& url = entry->GetURL();
    const GURL& virtual_url = entry->GetVirtualURL();
    if ((url.SchemeIs(content::kChromeUIScheme) &&
         url.host_piece() == chrome::kChromeUINewTabHost) ||
        (virtual_url.SchemeIs(content::kChromeUIScheme) &&
         virtual_url.host_piece() == chrome::kChromeUINewTabHost)) {
      return true;
    }
  }

  return search::NavEntryIsInstantNTP(source, entry);
}
