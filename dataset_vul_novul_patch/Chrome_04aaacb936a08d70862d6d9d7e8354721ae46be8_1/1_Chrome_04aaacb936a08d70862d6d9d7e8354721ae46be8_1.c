AppCache::AppCache(AppCacheStorage* storage, int64_t cache_id)
    : cache_id_(cache_id),
      owning_group_(nullptr),
       online_whitelist_all_(false),
       is_complete_(false),
       cache_size_(0),
       storage_(storage) {
   storage_->working_set()->AddCache(this);
 }
