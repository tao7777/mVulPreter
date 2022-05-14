 WebRunnerContentBrowserClient::CreateBrowserMainParts(
     const content::MainFunctionParams& parameters) {
   DCHECK(context_channel_);
  main_parts_ = new WebRunnerBrowserMainParts(std::move(context_channel_));
  return main_parts_;
 }
