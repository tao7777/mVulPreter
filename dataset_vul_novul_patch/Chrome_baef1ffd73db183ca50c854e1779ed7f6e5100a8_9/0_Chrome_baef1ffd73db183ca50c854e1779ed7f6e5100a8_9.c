  void SetUpCacheMetadata() {
     metadata_.reset(new GDataCacheMetadataMap(
         NULL, base::SequencedWorkerPool::SequenceToken()));
