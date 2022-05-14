  void SetUpCacheMetadata() {
     metadata_.reset(new GDataCacheMetadataMap(
         NULL, base::SequencedWorkerPool::SequenceToken()));
    metadata_->Initialize(cache_paths_);
  }
