   virtual void SetUp() {
    client_.Start(kClientId, kClientInfo, kState,
                  InvalidationVersionMap(),
                  MakeWeakHandle(mock_invalidation_state_tracker_.AsWeakPtr()),
                  &mock_listener_);
   }
