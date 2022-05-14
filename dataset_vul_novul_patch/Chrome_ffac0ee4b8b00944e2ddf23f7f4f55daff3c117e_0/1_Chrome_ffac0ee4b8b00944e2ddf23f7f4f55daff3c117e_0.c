 void ManifestManager::FetchManifest() {
   manifest_url_ = render_frame()->GetWebFrame()->GetDocument().ManifestURL();
 
   if (manifest_url_.is_empty()) {
    ManifestUmaUtil::FetchFailed(ManifestUmaUtil::FETCH_EMPTY_URL);
    ResolveCallbacks(ResolveStateFailure);
    return;
  }

  fetcher_.reset(new ManifestFetcher(manifest_url_));
  fetcher_->Start(
      render_frame()->GetWebFrame(),
      render_frame()->GetWebFrame()->GetDocument().ManifestUseCredentials(),
      base::Bind(&ManifestManager::OnManifestFetchComplete,
                 base::Unretained(this),
                 render_frame()->GetWebFrame()->GetDocument().Url()));
}
