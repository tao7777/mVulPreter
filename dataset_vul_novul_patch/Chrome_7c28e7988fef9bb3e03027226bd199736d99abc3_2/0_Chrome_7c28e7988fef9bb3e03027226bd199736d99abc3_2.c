void HostCache::Set(const Key& key,
                    const Entry& entry,
                    base::TimeTicks now,
                    base::TimeDelta ttl) {
  TRACE_EVENT0(kNetTracingCategory, "HostCache::Set");
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   if (caching_is_disabled())
     return;
 
  bool result_changed = false;
   auto it = entries_.find(key);
   if (it != entries_.end()) {
     bool is_stale = it->second.IsStale(now, network_changes_);
    AddressListDeltaType delta =
        FindAddressListDeltaType(it->second.addresses(), entry.addresses());
     RecordSet(is_stale ? SET_UPDATE_STALE : SET_UPDATE_VALID, now, &it->second,
              entry, delta);
    result_changed =
        entry.error() == OK &&
        (it->second.error() != entry.error() || delta != DELTA_IDENTICAL);
     entries_.erase(it);
   } else {
    result_changed = true;
     if (size() == max_entries_)
       EvictOneEntry(now);
    RecordSet(SET_INSERT, now, nullptr, entry, DELTA_DISJOINT);
   }
 
   AddEntry(Key(key), Entry(entry, now, ttl, network_changes_));

  if (delegate_ && result_changed)
    delegate_->ScheduleWrite();
 }
