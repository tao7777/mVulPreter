 bool AppCache::AddOrModifyEntry(const GURL& url, const AppCacheEntry& entry) {
   std::pair<EntryMap::iterator, bool> ret =
       entries_.insert(EntryMap::value_type(url, entry));
 
  if (!ret.second)
     ret.first->second.add_types(entry.types());
  else
     cache_size_ += entry.response_size();  // New entry. Add to cache size.
   return ret.second;
 }
