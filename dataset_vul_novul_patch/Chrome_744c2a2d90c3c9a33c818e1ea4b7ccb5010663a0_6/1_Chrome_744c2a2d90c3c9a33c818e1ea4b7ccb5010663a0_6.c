   virtual bool IsURLAcceptableForWebUI(
       BrowserContext* browser_context, const GURL& url) const {
    return HasWebUIScheme(url);
   }
