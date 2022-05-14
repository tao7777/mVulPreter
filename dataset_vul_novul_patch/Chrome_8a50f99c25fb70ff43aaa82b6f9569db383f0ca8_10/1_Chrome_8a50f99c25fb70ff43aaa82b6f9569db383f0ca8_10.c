  void FireInvalidate(const char* type_name,
                       int64 version, const char* payload) {
    const invalidation::ObjectId object_id(
        ipc::invalidation::ObjectSource::CHROME_SYNC, type_name);
    std::string payload_tmp = payload ? payload : "";
     invalidation::Invalidation inv;
     if (payload) {
       inv = invalidation::Invalidation(object_id, version, payload);
     } else {
       inv = invalidation::Invalidation(object_id, version);
     }
    invalidation::AckHandle ack_handle("fakedata");
    EXPECT_CALL(mock_invalidation_client_, Acknowledge(ack_handle));
    client_.Invalidate(&mock_invalidation_client_, inv, ack_handle);
     message_loop_.RunAllPending();
   }
