void SyncerUtil::UpdateServerFieldsFromUpdate(
    MutableEntry* target,
    const SyncEntity& update,
    const std::string& name) {
  if (update.deleted()) {
    if (target->Get(SERVER_IS_DEL)) {
      return;
    }
    target->Put(SERVER_IS_DEL, true);
    if (!target->Get(UNIQUE_CLIENT_TAG).empty()) {
      target->Put(SERVER_VERSION, 0);
    } else {
      target->Put(SERVER_VERSION,
          std::max(target->Get(SERVER_VERSION),
                   target->Get(BASE_VERSION)) + 1);
    }
    target->Put(IS_UNAPPLIED_UPDATE, true);
    return;
  }

  DCHECK(target->Get(ID) == update.id())
      << "ID Changing not supported here";
   target->Put(SERVER_PARENT_ID, update.parent_id());
   target->Put(SERVER_NON_UNIQUE_NAME, name);
   target->Put(SERVER_VERSION, update.version());
  target->Put(SERVER_CTIME, ProtoTimeToTime(update.ctime()));
  target->Put(SERVER_MTIME, ProtoTimeToTime(update.mtime()));
   target->Put(SERVER_IS_DIR, update.IsFolder());
   if (update.has_server_defined_unique_tag()) {
     const std::string& tag = update.server_defined_unique_tag();
    target->Put(UNIQUE_SERVER_TAG, tag);
  }
  if (update.has_client_defined_unique_tag()) {
    const std::string& tag = update.client_defined_unique_tag();
    target->Put(UNIQUE_CLIENT_TAG, tag);
  }
  if (update.has_specifics()) {
    DCHECK(update.GetModelType() != syncable::UNSPECIFIED)
        << "Storing unrecognized datatype in sync database.";
    target->Put(SERVER_SPECIFICS, update.specifics());
  } else if (update.has_bookmarkdata()) {
    const SyncEntity::BookmarkData& bookmark = update.bookmarkdata();
    UpdateBookmarkSpecifics(update.server_defined_unique_tag(),
                            bookmark.bookmark_url(),
                            bookmark.bookmark_favicon(),
                            target);
  }
  if (update.has_position_in_parent())
    target->Put(SERVER_POSITION_IN_PARENT, update.position_in_parent());

  target->Put(SERVER_IS_DEL, update.deleted());
  if (update.version() > target->Get(BASE_VERSION)) {
    target->Put(IS_UNAPPLIED_UPDATE, true);
  }
}
