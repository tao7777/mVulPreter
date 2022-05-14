 void AppCache::RemoveEntry(const GURL& url) {
   auto found = entries_.find(url);
   DCHECK(found != entries_.end());
   cache_size_ -= found->second.response_size();
   entries_.erase(found);
 }
