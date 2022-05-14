 void HostCache::clear() {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   RecordEraseAll(ERASE_CLEAR, base::TimeTicks::Now());
   entries_.clear();
 }
