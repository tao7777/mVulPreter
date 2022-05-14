void TopSitesImpl::SetTopSites(const MostVisitedURLList& new_top_sites,
                               const CallLocation location) {
  DCHECK(thread_checker_.CalledOnValidThread());

  MostVisitedURLList top_sites(new_top_sites);
  size_t num_forced_urls = MergeCachedForcedURLs(&top_sites);
  AddPrepopulatedPages(&top_sites, num_forced_urls);

  TopSitesDelta delta;
  DiffMostVisited(cache_->top_sites(), top_sites, &delta);

  TopSitesBackend::RecordHistogram record_or_not =
      TopSitesBackend::RECORD_HISTOGRAM_NO;

  if (location == CALL_LOCATION_FROM_ON_GOT_MOST_VISITED_THUMBNAILS &&
      !histogram_recorded_) {
    size_t delta_size =
        delta.deleted.size() + delta.added.size() + delta.moved.size();
    UMA_HISTOGRAM_COUNTS_100("History.FirstSetTopSitesDeltaSize", delta_size);
    record_or_not = TopSitesBackend::RECORD_HISTOGRAM_YES;
    histogram_recorded_ = true;
  }

  bool should_notify_observers = false;
  if (!delta.deleted.empty() || !delta.added.empty() || !delta.moved.empty()) {
    backend_->UpdateTopSites(delta, record_or_not);
    should_notify_observers = true;
  }
  if (!should_notify_observers)
    should_notify_observers = DoTitlesDiffer(cache_->top_sites(), top_sites);

   cache_->SetTopSites(top_sites);
  cache_->ClearUnreferencedThumbnails();
 
  if (!temp_images_.empty()) {
    for (const MostVisitedURL& mv : top_sites) {
      const GURL& canonical_url = cache_->GetCanonicalURL(mv.url);
      for (TempImages::iterator it = temp_images_.begin();
           it != temp_images_.end(); ++it) {
        if (canonical_url == cache_->GetCanonicalURL(it->first)) {
          bool success = SetPageThumbnailEncoded(
              mv.url, it->second.thumbnail.get(), it->second.thumbnail_score);
          if (success) {
            UMA_HISTOGRAM_ENUMERATION("Thumbnails.AddedToTopSites",
                                      THUMBNAIL_PROMOTED_TEMP_TO_REGULAR,
                                      THUMBNAIL_EVENT_COUNT);
          }
          temp_images_.erase(it);
          break;
        }
      }
    }
  }

  if (top_sites.size() - num_forced_urls >= kNonForcedTopSitesNumber)
    temp_images_.clear();

  ResetThreadSafeCache();
  ResetThreadSafeImageCache();

  if (should_notify_observers) {
    if (location == CALL_LOCATION_FROM_FORCED_URLS)
      NotifyTopSitesChanged(TopSitesObserver::ChangeReason::FORCED_URL);
    else
      NotifyTopSitesChanged(TopSitesObserver::ChangeReason::MOST_VISITED);
  }

}
