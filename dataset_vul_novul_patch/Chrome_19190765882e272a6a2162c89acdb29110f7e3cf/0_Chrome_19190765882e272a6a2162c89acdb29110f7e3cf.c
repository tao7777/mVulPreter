void BuildCommitCommand::ExecuteImpl(SyncSession* session) {
  ClientToServerMessage message;
  message.set_share(session->context()->account_name());
  message.set_message_contents(ClientToServerMessage::COMMIT);

  CommitMessage* commit_message = message.mutable_commit();
  commit_message->set_cache_guid(
      session->write_transaction()->directory()->cache_guid());
  AddExtensionsActivityToMessage(session, commit_message);
  SyncerProtoUtil::AddRequestBirthday(
      session->write_transaction()->directory(), &message);

  std::map<Id, std::pair<int64, int64> > position_map;

  const vector<Id>& commit_ids = session->status_controller()->commit_ids();
  for (size_t i = 0; i < commit_ids.size(); i++) {
    Id id = commit_ids[i];
    SyncEntity* sync_entry =
        static_cast<SyncEntity*>(commit_message->add_entries());
    sync_entry->set_id(id);
    MutableEntry meta_entry(session->write_transaction(),
                            syncable::GET_BY_ID,
                            id);
    CHECK(meta_entry.good());
    meta_entry.Put(syncable::SYNCING, true);

    DCHECK(0 != session->routing_info().count(meta_entry.GetModelType()))
        << "Committing change to datatype that's not actively enabled.";

    string name = meta_entry.Get(syncable::NON_UNIQUE_NAME);
    CHECK(!name.empty());  // Make sure this isn't an update.
    TruncateUTF8ToByteSize(name, 255, &name);
    sync_entry->set_name(name);

    sync_entry->set_non_unique_name(name);

    if (!meta_entry.Get(syncable::UNIQUE_CLIENT_TAG).empty()) {
      sync_entry->set_client_defined_unique_tag(
          meta_entry.Get(syncable::UNIQUE_CLIENT_TAG));
    }

    Id new_parent_id;
    if (meta_entry.Get(syncable::IS_DEL) &&
        !meta_entry.Get(syncable::PARENT_ID).ServerKnows()) {
      new_parent_id = session->write_transaction()->root_id();
    } else {
      new_parent_id = meta_entry.Get(syncable::PARENT_ID);
    }
    sync_entry->set_parent_id(new_parent_id);

    if (new_parent_id != meta_entry.Get(syncable::SERVER_PARENT_ID) &&
        0 != meta_entry.Get(syncable::BASE_VERSION) &&
        syncable::CHANGES_VERSION != meta_entry.Get(syncable::BASE_VERSION)) {
      sync_entry->set_old_parent_id(meta_entry.Get(syncable::SERVER_PARENT_ID));
    }

    int64 version = meta_entry.Get(syncable::BASE_VERSION);
    if (syncable::CHANGES_VERSION == version || 0 == version) {
      DCHECK(!id.ServerKnows() ||
             !meta_entry.Get(syncable::UNIQUE_CLIENT_TAG).empty())
          << meta_entry;

      sync_entry->set_version(0);
    } else {
       DCHECK(id.ServerKnows()) << meta_entry;
       sync_entry->set_version(meta_entry.Get(syncable::BASE_VERSION));
     }
    sync_entry->set_ctime(ClientTimeToServerTime(
        meta_entry.Get(syncable::CTIME)));
    sync_entry->set_mtime(ClientTimeToServerTime(
        meta_entry.Get(syncable::MTIME)));
 
     if (meta_entry.Get(IS_DEL)) {
      sync_entry->set_deleted(true);
    } else {
      if (meta_entry.Get(SPECIFICS).HasExtension(sync_pb::bookmark)) {
        const Id& prev_id = meta_entry.Get(syncable::PREV_ID);
        string prev_id_string =
            prev_id.IsRoot() ? string() : prev_id.GetServerId();
        sync_entry->set_insert_after_item_id(prev_id_string);

        std::pair<int64, int64> position_block(
            GetFirstPosition(), GetLastPosition());
        std::map<Id, std::pair<int64, int64> >::iterator prev_pos =
            position_map.find(prev_id);
        if (prev_pos != position_map.end()) {
          position_block = prev_pos->second;
          position_map.erase(prev_pos);
        } else {
          position_block = std::make_pair(
              FindAnchorPosition(syncable::PREV_ID, meta_entry),
              FindAnchorPosition(syncable::NEXT_ID, meta_entry));
        }
        position_block.first = InterpolatePosition(position_block.first,
                                                   position_block.second);

        position_map[id] = position_block;
        sync_entry->set_position_in_parent(position_block.first);
      }
      SetEntrySpecifics(&meta_entry, sync_entry);
    }
  }
  session->status_controller()->mutable_commit_message()->CopyFrom(message);
}
