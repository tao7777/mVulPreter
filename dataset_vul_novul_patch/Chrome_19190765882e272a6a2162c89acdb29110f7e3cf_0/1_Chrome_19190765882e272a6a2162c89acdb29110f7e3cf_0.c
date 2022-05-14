void ProcessCommitResponseCommand::UpdateServerFieldsAfterCommit(
    const sync_pb::SyncEntity& committed_entry,
    const CommitResponse_EntryResponse& entry_response,
    syncable::MutableEntry* local_entry) {


  local_entry->Put(SERVER_IS_DEL, committed_entry.deleted());
  if (committed_entry.deleted()) {
    return;
  }

  local_entry->Put(syncable::SERVER_IS_DIR,
      (committed_entry.folder() ||
       committed_entry.bookmarkdata().bookmark_folder()));
   local_entry->Put(syncable::SERVER_SPECIFICS,
       committed_entry.specifics());
   local_entry->Put(syncable::SERVER_MTIME,
                   ProtoTimeToTime(committed_entry.mtime()));
   local_entry->Put(syncable::SERVER_CTIME,
                   ProtoTimeToTime(committed_entry.ctime()));
   local_entry->Put(syncable::SERVER_POSITION_IN_PARENT,
       entry_response.position_in_parent());
  local_entry->Put(syncable::SERVER_PARENT_ID,
      local_entry->Get(syncable::PARENT_ID));
  local_entry->Put(syncable::SERVER_NON_UNIQUE_NAME,
      GetResultingPostCommitName(committed_entry, entry_response));

  if (local_entry->Get(IS_UNAPPLIED_UPDATE)) {
    local_entry->Put(IS_UNAPPLIED_UPDATE, false);
  }
}
