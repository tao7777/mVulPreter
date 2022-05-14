void SavePackage::OnReceivedSavableResourceLinksForCurrentPage(
    const std::vector<GURL>& resources_list,
    const std::vector<Referrer>& referrers_list,
    const std::vector<GURL>& frames_list) {
   if (wait_state_ != RESOURCES_LIST)
     return;
 
  if (resources_list.size() != referrers_list.size())
    return;

   all_save_items_count_ = static_cast<int>(resources_list.size()) +
                            static_cast<int>(frames_list.size());
 
  if (download_ && download_->IsInProgress())
    download_->SetTotalBytes(all_save_items_count_);

  if (all_save_items_count_) {
    for (int i = 0; i < static_cast<int>(resources_list.size()); ++i) {
      const GURL& u = resources_list[i];
      DCHECK(u.is_valid());
      SaveFileCreateInfo::SaveFileSource save_source = u.SchemeIsFile() ?
          SaveFileCreateInfo::SAVE_FILE_FROM_FILE :
          SaveFileCreateInfo::SAVE_FILE_FROM_NET;
      SaveItem* save_item = new SaveItem(u, referrers_list[i],
                                         this, save_source);
      waiting_item_queue_.push(save_item);
    }
    for (int i = 0; i < static_cast<int>(frames_list.size()); ++i) {
      const GURL& u = frames_list[i];
      DCHECK(u.is_valid());
      SaveItem* save_item = new SaveItem(
          u, Referrer(), this, SaveFileCreateInfo::SAVE_FILE_FROM_DOM);
      waiting_item_queue_.push(save_item);
    }
    wait_state_ = NET_FILES;
    DoSavingProcess();
  } else {
    Cancel(true);
  }
}
