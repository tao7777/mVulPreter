void LocalSiteCharacteristicsWebContentsObserver::DidFinishNavigation(
    content::NavigationHandle* navigation_handle) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(navigation_handle);

  if (!navigation_handle->IsInMainFrame() ||
      navigation_handle->IsSameDocument()) {
    return;
  }

  first_time_title_set_ = false;
  first_time_favicon_set_ = false;

  if (!navigation_handle->HasCommitted())
    return;

  const url::Origin new_origin =
      url::Origin::Create(navigation_handle->GetURL());

  if (writer_ && new_origin == writer_origin_)
    return;

   writer_.reset();
   writer_origin_ = url::Origin();
 
  if (!navigation_handle->GetURL().SchemeIsHTTPOrHTTPS())
     return;
 
   Profile* profile =
      Profile::FromBrowserContext(web_contents()->GetBrowserContext());
  DCHECK(profile);
  SiteCharacteristicsDataStore* data_store =
      LocalSiteCharacteristicsDataStoreFactory::GetForProfile(profile);
  DCHECK(data_store);
  writer_ = data_store->GetWriterForOrigin(
      new_origin,
      ContentVisibilityToRCVisibility(web_contents()->GetVisibility()));

  if (TabLoadTracker::Get()->GetLoadingState(web_contents()) ==
      LoadingState::LOADED) {
    writer_->NotifySiteLoaded();
  }

  writer_origin_ = new_origin;
}
