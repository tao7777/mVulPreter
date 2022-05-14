bool SessionModelAssociator::UpdateAssociationsFromSyncModel(
    const sync_api::ReadNode& root,
    const sync_api::BaseTransaction* trans) {
  DCHECK(CalledOnValidThread());

  int64 id = root.GetFirstChildId();
  while (id != sync_api::kInvalidId) {
    sync_api::ReadNode sync_node(trans);
    if (!sync_node.InitByIdLookup(id)) {
      LOG(ERROR) << "Failed to fetch sync node for id " << id;
      return false;
    }
 
     const sync_pb::SessionSpecifics& specifics =
         sync_node.GetSessionSpecifics();
    const base::Time& modification_time = sync_node.GetModificationTime();
     if (specifics.session_tag() != GetCurrentMachineTag()) {
       if (!AssociateForeignSpecifics(specifics, modification_time)) {
         return false;
      }
    } else if (id != local_session_syncid_) {
      if (specifics.has_header()) {
        if (sync_api::kInvalidId != local_session_syncid_)
          return false;

        local_session_syncid_ = id;
        if (specifics.header().has_client_name()) {
          current_session_name_ = specifics.header().client_name();
        }
      } else {
        if (!specifics.has_tab())
          return false;

        tab_pool_.AddTabNode(id);
      }
    }

    id = sync_node.GetSuccessorId();
  }

  if (!tab_pool_.full())
    return false;

  return true;
}
