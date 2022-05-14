ConflictResolver::ProcessSimpleConflict(WriteTransaction* trans,
                                        const Id& id,
                                        const Cryptographer* cryptographer,
                                        StatusController* status) {
  MutableEntry entry(trans, syncable::GET_BY_ID, id);
  CHECK(entry.good());

  if (!entry.Get(syncable::IS_UNAPPLIED_UPDATE) ||
      !entry.Get(syncable::IS_UNSYNCED)) {
    return NO_SYNC_PROGRESS;
  }

  if (entry.Get(syncable::IS_DEL) && entry.Get(syncable::SERVER_IS_DEL)) {
    entry.Put(syncable::IS_UNSYNCED, false);
    entry.Put(syncable::IS_UNAPPLIED_UPDATE, false);
    return NO_SYNC_PROGRESS;
  }

  if (!entry.Get(syncable::SERVER_IS_DEL)) {
    bool name_matches = entry.Get(syncable::NON_UNIQUE_NAME) ==
                        entry.Get(syncable::SERVER_NON_UNIQUE_NAME);
    bool parent_matches = entry.Get(syncable::PARENT_ID) ==
                          entry.Get(syncable::SERVER_PARENT_ID);
    bool entry_deleted = entry.Get(syncable::IS_DEL);

    syncable::Id server_prev_id = entry.ComputePrevIdFromServerPosition(
        entry.Get(syncable::SERVER_PARENT_ID));
    bool needs_reinsertion = !parent_matches ||
         server_prev_id != entry.Get(syncable::PREV_ID);
    DVLOG_IF(1, needs_reinsertion) << "Insertion needed, server prev id "
        << " is " << server_prev_id << ", local prev id is "
        << entry.Get(syncable::PREV_ID);
    const sync_pb::EntitySpecifics& specifics =
        entry.Get(syncable::SPECIFICS);
    const sync_pb::EntitySpecifics& server_specifics =
        entry.Get(syncable::SERVER_SPECIFICS);
    const sync_pb::EntitySpecifics& base_server_specifics =
        entry.Get(syncable::BASE_SERVER_SPECIFICS);
    std::string decrypted_specifics, decrypted_server_specifics;
    bool specifics_match = false;
    bool server_encrypted_with_default_key = false;
    if (specifics.has_encrypted()) {
      DCHECK(cryptographer->CanDecryptUsingDefaultKey(specifics.encrypted()));
      decrypted_specifics = cryptographer->DecryptToString(
          specifics.encrypted());
    } else {
      decrypted_specifics = specifics.SerializeAsString();
    }
    if (server_specifics.has_encrypted()) {
      server_encrypted_with_default_key =
          cryptographer->CanDecryptUsingDefaultKey(
              server_specifics.encrypted());
      decrypted_server_specifics = cryptographer->DecryptToString(
          server_specifics.encrypted());
    } else {
      decrypted_server_specifics = server_specifics.SerializeAsString();
    }
    if (decrypted_server_specifics == decrypted_specifics &&
        server_encrypted_with_default_key == specifics.has_encrypted()) {
      specifics_match = true;
    }
    bool base_server_specifics_match = false;
    if (server_specifics.has_encrypted() &&
        IsRealDataType(GetModelTypeFromSpecifics(base_server_specifics))) {
      std::string decrypted_base_server_specifics;
      if (!base_server_specifics.has_encrypted()) {
        decrypted_base_server_specifics =
            base_server_specifics.SerializeAsString();
      } else {
        decrypted_base_server_specifics = cryptographer->DecryptToString(
            base_server_specifics.encrypted());
      }
      if (decrypted_server_specifics == decrypted_base_server_specifics)
          base_server_specifics_match = true;
    }

    if (entry.GetModelType() == syncable::NIGORI) {
      sync_pb::EntitySpecifics specifics =
          entry.Get(syncable::SERVER_SPECIFICS);
      sync_pb::NigoriSpecifics* server_nigori = specifics.mutable_nigori();
      cryptographer->UpdateNigoriFromEncryptedTypes(server_nigori);
      if (cryptographer->is_ready()) {
        cryptographer->GetKeys(server_nigori->mutable_encrypted());
        server_nigori->set_using_explicit_passphrase(
             entry.Get(syncable::SPECIFICS).nigori().
                 using_explicit_passphrase());
       }
       entry.Put(syncable::SPECIFICS, specifics);
      DVLOG(1) << "Resolving simple conflict, merging nigori nodes: " << entry;
      status->increment_num_server_overwrites();
      OverwriteServerChanges(trans, &entry);
      UMA_HISTOGRAM_ENUMERATION("Sync.ResolveSimpleConflict",
                                NIGORI_MERGE,
                                CONFLICT_RESOLUTION_SIZE);
    } else if (!entry_deleted && name_matches && parent_matches &&
               specifics_match && !needs_reinsertion) {
      DVLOG(1) << "Resolving simple conflict, everything matches, ignoring "
               << "changes for: " << entry;
      OverwriteServerChanges(trans, &entry);
      IgnoreLocalChanges(&entry);
      UMA_HISTOGRAM_ENUMERATION("Sync.ResolveSimpleConflict",
                                CHANGES_MATCH,
                                CONFLICT_RESOLUTION_SIZE);
    } else if (base_server_specifics_match) {
      DVLOG(1) << "Resolving simple conflict, ignoring server encryption "
               << " changes for: " << entry;
      status->increment_num_server_overwrites();
      OverwriteServerChanges(trans, &entry);
      UMA_HISTOGRAM_ENUMERATION("Sync.ResolveSimpleConflict",
                                IGNORE_ENCRYPTION,
                                CONFLICT_RESOLUTION_SIZE);
    } else if (entry_deleted || !name_matches || !parent_matches) {
      OverwriteServerChanges(trans, &entry);
      status->increment_num_server_overwrites();
      DVLOG(1) << "Resolving simple conflict, overwriting server changes "
               << "for: " << entry;
      UMA_HISTOGRAM_ENUMERATION("Sync.ResolveSimpleConflict",
                                OVERWRITE_SERVER,
                                CONFLICT_RESOLUTION_SIZE);
    } else {
      DVLOG(1) << "Resolving simple conflict, ignoring local changes for: "
               << entry;
      IgnoreLocalChanges(&entry);
      status->increment_num_local_overwrites();
      UMA_HISTOGRAM_ENUMERATION("Sync.ResolveSimpleConflict",
                                OVERWRITE_LOCAL,
                                CONFLICT_RESOLUTION_SIZE);
    }
    entry.Put(syncable::BASE_SERVER_SPECIFICS, sync_pb::EntitySpecifics());
    return SYNC_PROGRESS;
  } else {  // SERVER_IS_DEL is true
    if (entry.Get(syncable::IS_DIR)) {
      Directory::ChildHandles children;
      trans->directory()->GetChildHandlesById(trans,
                                              entry.Get(syncable::ID),
                                              &children);
      if (0 != children.size()) {
        DVLOG(1) << "Entry is a server deleted directory with local contents, "
                 << "should be a hierarchy conflict. (race condition).";
        return NO_SYNC_PROGRESS;
      }
    }

    if (!entry.Get(syncable::UNIQUE_CLIENT_TAG).empty()) {
      DCHECK_EQ(entry.Get(syncable::SERVER_VERSION), 0) << "For the server to "
          "know to re-create, client-tagged items should revert to version 0 "
          "when server-deleted.";
      OverwriteServerChanges(trans, &entry);
      status->increment_num_server_overwrites();
      DVLOG(1) << "Resolving simple conflict, undeleting server entry: "
               << entry;
      UMA_HISTOGRAM_ENUMERATION("Sync.ResolveSimpleConflict",
                                OVERWRITE_SERVER,
                                CONFLICT_RESOLUTION_SIZE);
      entry.Put(syncable::SERVER_VERSION, 0);
      entry.Put(syncable::BASE_VERSION, 0);
    } else {
      SyncerUtil::SplitServerInformationIntoNewEntry(trans, &entry);

      MutableEntry server_update(trans, syncable::GET_BY_ID, id);
      CHECK(server_update.good());
      CHECK(server_update.Get(syncable::META_HANDLE) !=
            entry.Get(syncable::META_HANDLE))
          << server_update << entry;
      UMA_HISTOGRAM_ENUMERATION("Sync.ResolveSimpleConflict",
                                UNDELETE,
                                CONFLICT_RESOLUTION_SIZE);
    }
    return SYNC_PROGRESS;
  }
}
