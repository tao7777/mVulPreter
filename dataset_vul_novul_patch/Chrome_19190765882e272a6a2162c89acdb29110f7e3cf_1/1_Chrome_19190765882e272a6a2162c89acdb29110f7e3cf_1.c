 void ClearServerData(syncable::MutableEntry* entry) {
   entry->Put(SERVER_NON_UNIQUE_NAME, "");
   entry->Put(SERVER_PARENT_ID, syncable::GetNullId());
  entry->Put(SERVER_MTIME, Time());
  entry->Put(SERVER_CTIME, Time());
   entry->Put(SERVER_VERSION, 0);
   entry->Put(SERVER_IS_DIR, false);
   entry->Put(SERVER_IS_DEL, false);
  entry->Put(IS_UNAPPLIED_UPDATE, false);
  entry->Put(SERVER_SPECIFICS, sync_pb::EntitySpecifics::default_instance());
  entry->Put(SERVER_POSITION_IN_PARENT, 0);
}
