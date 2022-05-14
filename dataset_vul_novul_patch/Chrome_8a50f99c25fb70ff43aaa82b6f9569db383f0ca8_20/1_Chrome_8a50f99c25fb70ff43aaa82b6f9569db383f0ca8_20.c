void RegistrationManager::SetRegisteredIds(const ObjectIdSet& ids) {
   DCHECK(CalledOnValidThread());
 
   const ObjectIdSet& old_ids = GetRegisteredIds();
  const ObjectIdSet& to_register = ids;
  ObjectIdSet to_unregister;
  std::set_difference(old_ids.begin(), old_ids.end(),
                      ids.begin(), ids.end(),
                      std::inserter(to_unregister, to_unregister.begin()),
                      ObjectIdLessThan());

  for (ObjectIdSet::const_iterator it = to_unregister.begin();
       it != to_unregister.end(); ++it) {
    UnregisterId(*it);
  }

  for (ObjectIdSet::const_iterator it = to_register.begin();
       it != to_register.end(); ++it) {
    if (!ContainsKey(registration_statuses_, *it)) {
      registration_statuses_.insert(
          std::make_pair(*it, new RegistrationStatus(*it, this)));
    }
    if (!IsIdRegistered(*it)) {
      TryRegisterId(*it, false /* is-retry */);
    }
  }
}
