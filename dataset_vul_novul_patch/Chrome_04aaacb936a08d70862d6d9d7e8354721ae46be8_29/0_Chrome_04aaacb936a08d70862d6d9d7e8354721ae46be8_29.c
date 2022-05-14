 void AppCacheUpdateJob::OnManifestDataWriteComplete(int result) {
   if (result > 0) {
    // The manifest determines the cache's origin, so the manifest entry is
    // always same-origin, and thus does not require padding.
     AppCacheEntry entry(AppCacheEntry::MANIFEST,
                        manifest_response_writer_->response_id(),
                        manifest_response_writer_->amount_written(),
                        /*padding_size=*/0);
     if (!inprogress_cache_->AddOrModifyEntry(manifest_url_, entry))
       duplicate_response_ids_.push_back(entry.response_id());
     StoreGroupAndCache();
  } else {
    HandleCacheFailure(
        blink::mojom::AppCacheErrorDetails(
            "Failed to write the manifest data to storage",
            blink::mojom::AppCacheErrorReason::APPCACHE_UNKNOWN_ERROR, GURL(),
            0, false /*is_cross_origin*/),
        DISKCACHE_ERROR, GURL());
  }
}
