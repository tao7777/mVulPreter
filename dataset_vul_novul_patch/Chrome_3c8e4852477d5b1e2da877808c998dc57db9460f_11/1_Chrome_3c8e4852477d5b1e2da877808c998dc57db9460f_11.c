 void NetworkHandler::ClearBrowserCache(
     std::unique_ptr<ClearBrowserCacheCallback> callback) {
  if (!process_) {
     callback->sendFailure(Response::InternalError());
     return;
   }
   content::BrowsingDataRemover* remover =
      content::BrowserContext::GetBrowsingDataRemover(
          process_->GetBrowserContext());
   remover->RemoveAndReply(
       base::Time(), base::Time::Max(),
       content::BrowsingDataRemover::DATA_TYPE_CACHE,
      content::BrowsingDataRemover::ORIGIN_TYPE_UNPROTECTED_WEB,
      new DevtoolsClearCacheObserver(remover, std::move(callback)));
}
