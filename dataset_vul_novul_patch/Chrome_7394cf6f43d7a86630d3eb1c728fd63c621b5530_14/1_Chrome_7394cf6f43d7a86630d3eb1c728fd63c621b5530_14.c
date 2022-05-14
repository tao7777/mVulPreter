   std::unique_ptr<WebContents> CreateWebContents() {
     std::unique_ptr<WebContents> web_contents = CreateTestWebContents();
     content::WebContentsTester::For(web_contents.get())
         ->NavigateAndCommit(GURL("https://www.example.com"));
     return web_contents;
   }
