void SyncManager::SyncInternal::MaybeSetSyncTabsInNigoriNode(
    const ModelTypeSet enabled_types) {
  if (initialized_ && enabled_types.Has(syncable::SESSIONS)) {
    WriteTransaction trans(FROM_HERE, GetUserShare());
    WriteNode node(&trans);
    if (node.InitByTagLookup(kNigoriTag) != sync_api::BaseNode::INIT_OK) {
      LOG(WARNING) << "Unable to set 'sync_tabs' bit because Nigori node not "
                   << "found.";
      return;
    }
    sync_pb::NigoriSpecifics specifics(node.GetNigoriSpecifics());
    specifics.set_sync_tabs(true);
    node.SetNigoriSpecifics(specifics);
  }
 }
