void InvalidationNotifier::UpdateCredentials(
     const std::string& email, const std::string& token) {
   if (state_ == STOPPED) {
     invalidation_client_.Start(
        base::Bind(&invalidation::CreateInvalidationClient),
         invalidation_client_id_, client_info_, invalidation_state_,
         initial_max_invalidation_versions_,
         invalidation_state_tracker_,
        this);
    invalidation_state_.clear();
    state_ = STARTED;
  }
  invalidation_client_.UpdateCredentials(email, token);
}
