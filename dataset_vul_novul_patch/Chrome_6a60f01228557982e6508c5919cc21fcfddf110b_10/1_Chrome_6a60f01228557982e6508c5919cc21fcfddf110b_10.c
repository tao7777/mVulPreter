 WebRunnerMainDelegate::CreateContentBrowserClient() {
   DCHECK(!browser_client_);
   browser_client_ = std::make_unique<WebRunnerContentBrowserClient>(
       std::move(context_channel_));
  return browser_client_.get();
}
