RefCountedMemory* ChromeWebUIControllerFactory::GetFaviconResourceBytes(
    const GURL& page_url) const {
  if (page_url.host() == extension_misc::kBookmarkManagerId)
    return BookmarksUI::GetFaviconResourceBytes();

  if (page_url.SchemeIs(chrome::kExtensionScheme)) {
    NOTREACHED();
     return NULL;
   }
 
  if (!HasWebUIScheme(page_url))
     return NULL;
 
 #if defined(OS_WIN)
  if (page_url.host() == chrome::kChromeUIConflictsHost)
    return ConflictsUI::GetFaviconResourceBytes();
#endif

  if (page_url.host() == chrome::kChromeUICrashesHost)
    return CrashesUI::GetFaviconResourceBytes();

  if (page_url.host() == chrome::kChromeUIHistoryHost)
    return HistoryUI::GetFaviconResourceBytes();

  if (page_url.host() == chrome::kChromeUIFlagsHost)
    return FlagsUI::GetFaviconResourceBytes();

  if (page_url.host() == chrome::kChromeUISessionsHost)
    return SessionsUI::GetFaviconResourceBytes();

  if (page_url.host() == chrome::kChromeUIFlashHost)
    return FlashUI::GetFaviconResourceBytes();

#if !defined(OS_ANDROID)
  if (page_url.host() == chrome::kChromeUIDownloadsHost)
    return DownloadsUI::GetFaviconResourceBytes();

  if (page_url.host() == chrome::kChromeUISettingsHost)
    return OptionsUI::GetFaviconResourceBytes();

  if (page_url.host() == chrome::kChromeUISettingsFrameHost)
    return options2::OptionsUI::GetFaviconResourceBytes();
#endif

  if (page_url.host() == chrome::kChromeUIPluginsHost)
    return PluginsUI::GetFaviconResourceBytes();

  return NULL;
}
