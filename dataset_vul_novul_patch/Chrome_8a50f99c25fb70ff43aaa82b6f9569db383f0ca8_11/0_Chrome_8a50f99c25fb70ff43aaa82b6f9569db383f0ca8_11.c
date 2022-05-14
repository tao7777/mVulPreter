   void FireInvalidateAll() {
    const AckHandle ack_handle("fakedata_all");
    fake_invalidation_client_->ClearAckedHandles();
    client_.InvalidateAll(fake_invalidation_client_, ack_handle);
    EXPECT_TRUE(fake_invalidation_client_->IsAckedHandle(ack_handle));
  }

  void WriteState(const std::string& new_state) {
    client_.WriteState(new_state);
    // Pump message loop to trigger
    // InvalidationStateTracker::WriteState().
    message_loop_.RunAllPending();
  }

  void EnableNotifications() {
    fake_push_client_->EnableNotifications();
  }

  void DisableNotifications(notifier::NotificationsDisabledReason reason) {
    fake_push_client_->DisableNotifications(reason);
  }

  const ObjectId kBookmarksId_;
  const ObjectId kPreferencesId_;
  const ObjectId kExtensionsId_;
  const ObjectId kAppsId_;

  ObjectIdSet registered_ids_;

 private:
  void StartClient() {
    fake_invalidation_client_ = NULL;
    client_.Start(base::Bind(&CreateFakeInvalidationClient,
                             &fake_invalidation_client_),
                  kClientId, kClientInfo, kState,
                  InvalidationVersionMap(),
                  MakeWeakHandle(fake_tracker_.AsWeakPtr()),
                  &fake_listener_);
    DCHECK(fake_invalidation_client_);
  }

  void StopClient() {
    // client_.StopForTest() stops the invalidation scheduler, which
    // deletes any pending tasks without running them.  Some tasks
    // "run and delete" another task, so they must be run in order to
    // avoid leaking the inner task.  client_.StopForTest() does not
    // schedule any tasks, so it's both necessary and sufficient to
    // drain the task queue before calling it.
    message_loop_.RunAllPending();
    fake_invalidation_client_ = NULL;
    client_.StopForTest();
   }
