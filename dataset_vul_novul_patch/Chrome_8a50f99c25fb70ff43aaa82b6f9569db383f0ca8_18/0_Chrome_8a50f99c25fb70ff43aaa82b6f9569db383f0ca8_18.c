 void InvalidationNotifier::UpdateRegisteredIds(SyncNotifierObserver* handler,
                                                const ObjectIdSet& ids) {
   DCHECK(CalledOnValidThread());
  const ObjectIdSet& all_registered_ids =
      helper_.UpdateRegisteredIds(handler, ids);
  invalidation_client_.UpdateRegisteredIds(all_registered_ids);
 }
