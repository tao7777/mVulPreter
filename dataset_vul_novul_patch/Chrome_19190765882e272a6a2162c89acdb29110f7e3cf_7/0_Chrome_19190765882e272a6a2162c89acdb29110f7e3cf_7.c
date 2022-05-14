void SessionChangeProcessor::ApplyChangesFromSyncModel(
    const sync_api::BaseTransaction* trans,
    const sync_api::ImmutableChangeRecordList& changes) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  if (!running()) {
    return;
  }

  StopObserving();

  sync_api::ReadNode root(trans);
  if (!root.InitByTagLookup(kSessionsTag)) {
    error_handler()->OnUnrecoverableError(FROM_HERE,
        "Sessions root node lookup failed.");
    return;
  }

  for (sync_api::ChangeRecordList::const_iterator it =
           changes.Get().begin(); it != changes.Get().end(); ++it) {
    const sync_api::ChangeRecord& change = *it;
    sync_api::ChangeRecord::Action action(change.action);
    if (sync_api::ChangeRecord::ACTION_DELETE == action) {
      sync_api::ReadNode node(trans);
      if (!node.InitByIdLookup(change.id)) {
        error_handler()->OnUnrecoverableError(FROM_HERE,
                                              "Session node lookup failed.");
        return;
      }
      DCHECK_EQ(node.GetModelType(), syncable::SESSIONS);
      const sync_pb::SessionSpecifics& specifics = node.GetSessionSpecifics();
      session_model_associator_->DisassociateForeignSession(
          specifics.session_tag());
      continue;
    }

    sync_api::ReadNode sync_node(trans);
    if (!sync_node.InitByIdLookup(change.id)) {
      error_handler()->OnUnrecoverableError(FROM_HERE,
          "Session node lookup failed.");
      return;
    }

    DCHECK(root.GetId() == sync_node.GetParentId());
    DCHECK(syncable::SESSIONS == sync_node.GetModelType());

    const sync_pb::SessionSpecifics& specifics(
        sync_node.GetSessionSpecifics());
    if (specifics.session_tag() ==
            session_model_associator_->GetCurrentMachineTag() &&
        !setup_for_test_) {
      LOG(WARNING) << "Dropping modification to local session.";
       StartObserving();
       return;
     }
    const int64 mtime = sync_node.GetModificationTime();
     session_model_associator_->AssociateForeignSpecifics(specifics, mtime);
   }

  NotificationService::current()->Notify(
      chrome::NOTIFICATION_FOREIGN_SESSION_UPDATED,
      NotificationService::AllSources(),
      NotificationService::NoDetails());

  StartObserving();
}
