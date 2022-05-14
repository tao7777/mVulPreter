 ContentUtilityClient* ShellMainDelegate::CreateContentUtilityClient() {
  utility_client_.reset(new ShellContentUtilityClient);
   return utility_client_.get();
 }
