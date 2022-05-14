void HostCache::Set(const Key& key,
                    const Entry& entry,
                    base::TimeTicks now,
                    base::TimeDelta ttl) {
  TRACE_EVENT0(kNetTracingCategory, "HostCache::Set");
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   if (caching_is_disabled())
     return;
 
   auto it = entries_.find(key);
   if (it != entries_.end()) {
     bool is_stale = it->second.IsStale(now, network_changes_);
     RecordSet(is_stale ? SET_UPDATE_STALE : SET_UPDATE_VALID, now, &it->second,
              entry);
     entries_.erase(it);
   } else {
     if (size() == max_entries_)
       EvictOneEntry(now);
    RecordSet(SET_INSERT, now, nullptr, entry);
   }
 
   AddEntry(Key(key), Entry(entry, now, ttl, network_changes_));
 }
