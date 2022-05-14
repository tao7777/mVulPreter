 ContentUtilityClient* ShellMainDelegate::CreateContentUtilityClient() {
  utility_client_.reset(new ShellContentUtilityClient(is_browsertest_));
   return utility_client_.get();
 }
