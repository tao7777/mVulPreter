void MemBackendImpl::EvictIfNeeded() {
  if (current_size_ <= max_size_)
    return;

  int target_size = std::max(0, max_size_ - kDefaultEvictionSize);

   base::LinkNode<MemEntryImpl>* entry = lru_list_.head();
   while (current_size_ > target_size && entry != lru_list_.end()) {
     MemEntryImpl* to_doom = entry->value();

    do {
      entry = entry->next();
      // It's possible that entry now points to a child of to_doom, and the
      // parent is about to be deleted. Skip past any child entries.
    } while (entry != lru_list_.end() && entry->value()->parent() == to_doom);

     if (!to_doom->InUse())
       to_doom->Doom();
   }
}
