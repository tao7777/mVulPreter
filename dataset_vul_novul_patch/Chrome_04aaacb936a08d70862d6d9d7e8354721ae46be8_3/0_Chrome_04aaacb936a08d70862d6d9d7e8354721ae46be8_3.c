 void AppCache::RemoveEntry(const GURL& url) {
   auto found = entries_.find(url);
   DCHECK(found != entries_.end());
  DCHECK_GE(cache_size_, found->second.response_size());
  DCHECK_GE(padding_size_, found->second.padding_size());
   cache_size_ -= found->second.response_size();
  padding_size_ -= found->second.padding_size();
   entries_.erase(found);
 }
