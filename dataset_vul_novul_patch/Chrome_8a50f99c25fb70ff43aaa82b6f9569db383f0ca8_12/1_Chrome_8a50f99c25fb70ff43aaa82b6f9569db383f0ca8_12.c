  void FireInvalidateUnknownVersion(const char* type_name) {
    const invalidation::ObjectId object_id(
        ipc::invalidation::ObjectSource::CHROME_SYNC, type_name);
    invalidation::AckHandle ack_handle("fakedata");
    EXPECT_CALL(mock_invalidation_client_, Acknowledge(ack_handle));
    client_.InvalidateUnknownVersion(&mock_invalidation_client_, object_id,
                                      ack_handle);
   }
