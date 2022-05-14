void MemBackendImpl::EvictIfNeeded() {
  if (current_size_ <= max_size_)
    return;

  int target_size = std::max(0, max_size_ - kDefaultEvictionSize);

   base::LinkNode<MemEntryImpl>* entry = lru_list_.head();
   while (current_size_ > target_size && entry != lru_list_.end()) {
     MemEntryImpl* to_doom = entry->value();
    entry = NextSkippingChildren(lru_list_, entry);
 
     if (!to_doom->InUse())
       to_doom->Doom();
  }
}
