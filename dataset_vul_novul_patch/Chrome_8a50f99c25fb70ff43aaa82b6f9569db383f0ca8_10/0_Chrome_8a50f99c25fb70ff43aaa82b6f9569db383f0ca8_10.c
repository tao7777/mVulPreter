  void FireInvalidate(const char* type_name,
  // Restart client without re-registering IDs.
  void RestartClient() {
    StopClient();
    StartClient();
  }

  int GetInvalidationCount(const ObjectId& id) const {
    return fake_listener_.GetInvalidationCount(id);
  }

  std::string GetPayload(const ObjectId& id) const {
    return fake_listener_.GetPayload(id);
  }

  NotificationsDisabledReason GetNotificationsDisabledReason() const {
    return fake_listener_.GetNotificationsDisabledReason();
  }

  int64 GetMaxVersion(const ObjectId& id) const {
    return fake_tracker_.GetMaxVersion(id);
  }

  std::string GetInvalidationState() const {
    return fake_tracker_.GetInvalidationState();
  }

  ObjectIdSet GetRegisteredIds() const {
    return fake_invalidation_client_->GetRegisteredIds();
  }

  // |payload| can be NULL.
  void FireInvalidate(const ObjectId& object_id,
                       int64 version, const char* payload) {
     invalidation::Invalidation inv;
     if (payload) {
       inv = invalidation::Invalidation(object_id, version, payload);
     } else {
       inv = invalidation::Invalidation(object_id, version);
     }
    const AckHandle ack_handle("fakedata");
    fake_invalidation_client_->ClearAckedHandles();
    client_.Invalidate(fake_invalidation_client_, inv, ack_handle);
    EXPECT_TRUE(fake_invalidation_client_->IsAckedHandle(ack_handle));
     message_loop_.RunAllPending();
   }
