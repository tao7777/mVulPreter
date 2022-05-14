 void ResourceFetcher::EnableIsPreloadedForTest() {
  if (preloaded_urls_for_test_)
     return;
  preloaded_urls_for_test_ = WTF::WrapUnique(new HashSet<String>);
 
   if (preloads_) {
     for (const auto& resource : *preloads_)
      preloaded_urls_for_test_->insert(resource->Url().GetString());
   }
 }
