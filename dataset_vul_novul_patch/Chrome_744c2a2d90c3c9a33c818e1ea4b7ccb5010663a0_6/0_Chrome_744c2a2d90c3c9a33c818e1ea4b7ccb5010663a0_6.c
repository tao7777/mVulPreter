   virtual bool IsURLAcceptableForWebUI(
       BrowserContext* browser_context, const GURL& url) const {
    return content::GetContentClient()->HasWebUIScheme(url);
  }
};

class TabContentsTestClient : public TestContentClient {
 public:
  TabContentsTestClient() {
  }

  virtual bool HasWebUIScheme(const GURL& url) const OVERRIDE {
    return url.SchemeIs("tabcontentstest");
   }
