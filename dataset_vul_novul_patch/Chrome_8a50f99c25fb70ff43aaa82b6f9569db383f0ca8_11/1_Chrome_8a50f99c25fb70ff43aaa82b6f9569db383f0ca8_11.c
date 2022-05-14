   void FireInvalidateAll() {
    invalidation::AckHandle ack_handle("fakedata");
    EXPECT_CALL(mock_invalidation_client_, Acknowledge(ack_handle));
    client_.InvalidateAll(&mock_invalidation_client_, ack_handle);
   }
