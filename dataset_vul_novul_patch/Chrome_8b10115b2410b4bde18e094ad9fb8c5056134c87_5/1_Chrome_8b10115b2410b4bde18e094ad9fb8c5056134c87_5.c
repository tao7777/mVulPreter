  static void Unregister(const GURL& url) {
    EXPECT_TRUE(
        content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
    net::URLRequestFilter::GetInstance()->RemoveUrlHandler(url);
   }
