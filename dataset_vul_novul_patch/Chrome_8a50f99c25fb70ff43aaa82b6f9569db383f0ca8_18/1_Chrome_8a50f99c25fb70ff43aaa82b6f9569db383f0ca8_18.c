 void InvalidationNotifier::UpdateRegisteredIds(SyncNotifierObserver* handler,
                                                const ObjectIdSet& ids) {
   DCHECK(CalledOnValidThread());
  invalidation_client_.RegisterIds(helper_.UpdateRegisteredIds(handler, ids));
 }
