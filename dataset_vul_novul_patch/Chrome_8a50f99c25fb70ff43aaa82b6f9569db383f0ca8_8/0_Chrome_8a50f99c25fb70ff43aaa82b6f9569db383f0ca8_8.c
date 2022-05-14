 void ChromeInvalidationClient::Start(
    const CreateInvalidationClientCallback&
        create_invalidation_client_callback,
     const std::string& client_id, const std::string& client_info,
     const std::string& state,
     const InvalidationVersionMap& initial_max_invalidation_versions,
    const WeakHandle<InvalidationStateTracker>& invalidation_state_tracker,
    Listener* listener) {
  DCHECK(CalledOnValidThread());
  Stop();

  chrome_system_resources_.set_platform(client_info);
  chrome_system_resources_.Start();

  chrome_system_resources_.storage()->SetInitialState(state);

  max_invalidation_versions_ = initial_max_invalidation_versions;
  if (max_invalidation_versions_.empty()) {
    DVLOG(2) << "No initial max invalidation versions for any id";
  } else {
    for (InvalidationVersionMap::const_iterator it =
             max_invalidation_versions_.begin();
         it != max_invalidation_versions_.end(); ++it) {
      DVLOG(2) << "Initial max invalidation version for "
               << ObjectIdToString(it->first) << " is "
               << it->second;
    }
  }
  invalidation_state_tracker_ = invalidation_state_tracker;
  DCHECK(invalidation_state_tracker_.IsInitialized());

  DCHECK(!listener_);
  DCHECK(listener);
  listener_ = listener;
 
   int client_type = ipc::invalidation::ClientType::CHROME_SYNC;
   invalidation_client_.reset(
      create_invalidation_client_callback.Run(
           &chrome_system_resources_, client_type, client_id,
           kApplicationName, this));
   invalidation_client_->Start();

  registration_manager_.reset(
      new RegistrationManager(invalidation_client_.get()));
}
