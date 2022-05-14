 void HostCache::clear() {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   RecordEraseAll(ERASE_CLEAR, base::TimeTicks::Now());

  // Don't bother scheduling a write if there's nothing to clear.
  if (size() == 0)
    return;

   entries_.clear();
  if (delegate_)
    delegate_->ScheduleWrite();
 }
