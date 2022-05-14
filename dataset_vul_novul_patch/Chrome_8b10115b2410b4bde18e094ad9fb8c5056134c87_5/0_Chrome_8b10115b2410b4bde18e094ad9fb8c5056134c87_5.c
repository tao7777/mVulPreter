  static void Unregister(const GURL& url) {
  void WillStartRequest(network::ResourceRequest* request,
                        bool* defer) override {
    request->headers.SetHeader(signin::kChromeConnectedHeader, "User Data");
   }
