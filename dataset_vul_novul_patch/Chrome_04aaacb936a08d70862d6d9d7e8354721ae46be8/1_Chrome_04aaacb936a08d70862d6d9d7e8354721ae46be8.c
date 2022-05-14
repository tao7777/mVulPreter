void AppCache::AddEntry(const GURL& url, const AppCacheEntry& entry) {
   DCHECK(entries_.find(url) == entries_.end());
   entries_.insert(EntryMap::value_type(url, entry));
   cache_size_ += entry.response_size();
 }
