 void HostCache::ClearForHosts(
    const base::Callback<bool(const std::string&)>& host_filter) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

  if (host_filter.is_null()) {
    clear();
     return;
   }
 
  bool changed = false;
   base::TimeTicks now = base::TimeTicks::Now();
   for (EntryMap::iterator it = entries_.begin(); it != entries_.end();) {
     EntryMap::iterator next_it = std::next(it);
 
     if (host_filter.Run(it->first.hostname)) {
       RecordErase(ERASE_CLEAR, now, it->second);
       entries_.erase(it);
      changed = true;
     }
 
     it = next_it;
   }

  if (delegate_ && changed)
    delegate_->ScheduleWrite();
 }
