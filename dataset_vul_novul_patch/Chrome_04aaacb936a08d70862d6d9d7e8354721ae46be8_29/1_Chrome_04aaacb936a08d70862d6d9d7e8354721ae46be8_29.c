 void AppCacheUpdateJob::OnManifestDataWriteComplete(int result) {
   if (result > 0) {
     AppCacheEntry entry(AppCacheEntry::MANIFEST,
        manifest_response_writer_->response_id(),
        manifest_response_writer_->amount_written());
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
