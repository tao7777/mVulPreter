void InitializePrinting(content::WebContents* web_contents) {
#if BUILDFLAG(ENABLE_PRINT_PREVIEW)
  printing::PrintViewManager::CreateForWebContents(web_contents);
  printing::PrintPreviewMessageHandler::CreateForWebContents(web_contents);
 #else
   printing::PrintViewManagerBasic::CreateForWebContents(web_contents);
 #endif  // BUILDFLAG(ENABLE_PRINT_PREVIEW)
 }
