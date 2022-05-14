 WebRunnerContentBrowserClient::CreateBrowserMainParts(
     const content::MainFunctionParams& parameters) {
   DCHECK(context_channel_);
  return new WebRunnerBrowserMainParts(std::move(context_channel_));
 }
