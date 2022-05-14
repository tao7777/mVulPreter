int MemBackendImpl::DoomEntriesBetween(Time initial_time,
                                       Time end_time,
                                       const CompletionCallback& callback) {
  if (end_time.is_null())
    end_time = Time::Max();
  DCHECK_GE(end_time, initial_time);

  base::LinkNode<MemEntryImpl>* node = lru_list_.head();
  while (node != lru_list_.end() && node->value()->GetLastUsed() < initial_time)
     node = node->next();
   while (node != lru_list_.end() && node->value()->GetLastUsed() < end_time) {
     MemEntryImpl* to_doom = node->value();
    node = NextSkippingChildren(lru_list_, node);
     to_doom->Doom();
   }
 
  return net::OK;
}
