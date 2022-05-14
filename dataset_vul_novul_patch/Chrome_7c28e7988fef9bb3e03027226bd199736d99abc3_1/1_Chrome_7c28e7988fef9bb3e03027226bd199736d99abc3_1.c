 void HostCache::RecordSet(SetOutcome outcome,
                           base::TimeTicks now,
                           const Entry* old_entry,
                          const Entry& new_entry) {
   CACHE_HISTOGRAM_ENUM("Set", outcome, MAX_SET_OUTCOME);
   switch (outcome) {
     case SET_INSERT:
    case SET_UPDATE_VALID:
      break;
    case SET_UPDATE_STALE: {
      EntryStaleness stale;
      old_entry->GetStaleness(now, network_changes_, &stale);
      CACHE_HISTOGRAM_TIME("UpdateStale.ExpiredBy", stale.expired_by);
      CACHE_HISTOGRAM_COUNT("UpdateStale.NetworkChanges",
                             stale.network_changes);
       CACHE_HISTOGRAM_COUNT("UpdateStale.StaleHits", stale.stale_hits);
       if (old_entry->error() == OK && new_entry.error() == OK) {
        AddressListDeltaType delta = FindAddressListDeltaType(
            old_entry->addresses(), new_entry.addresses());
         RecordUpdateStale(delta, stale);
       }
       break;
    }
    case MAX_SET_OUTCOME:
      NOTREACHED();
      break;
  }
}
