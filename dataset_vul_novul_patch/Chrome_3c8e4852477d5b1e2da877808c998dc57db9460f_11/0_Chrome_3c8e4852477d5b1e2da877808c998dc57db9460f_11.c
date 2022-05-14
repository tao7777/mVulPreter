 void NetworkHandler::ClearBrowserCache(
     std::unique_ptr<ClearBrowserCacheCallback> callback) {
  if (!browser_context_) {
     callback->sendFailure(Response::InternalError());
     return;
   }
   content::BrowsingDataRemover* remover =
      content::BrowserContext::GetBrowsingDataRemover(browser_context_);
   remover->RemoveAndReply(
       base::Time(), base::Time::Max(),
       content::BrowsingDataRemover::DATA_TYPE_CACHE,
      content::BrowsingDataRemover::ORIGIN_TYPE_UNPROTECTED_WEB,
      new DevtoolsClearCacheObserver(remover, std::move(callback)));
}
