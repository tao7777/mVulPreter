  void FireInvalidateUnknownVersion(const char* type_name) {
  void FireInvalidateUnknownVersion(const ObjectId& object_id) {
    const AckHandle ack_handle("fakedata_unknown");
    fake_invalidation_client_->ClearAckedHandles();
    client_.InvalidateUnknownVersion(fake_invalidation_client_, object_id,
                                      ack_handle);
    EXPECT_TRUE(fake_invalidation_client_->IsAckedHandle(ack_handle));
   }
