  virtual void SetUp() {
     client_jid_ = "user@domain/rest-of-jid";
 
    default_screen_size_.set(1000, 1000);
    EXPECT_CALL(capturer_, size_most_recent())
        .WillRepeatedly(ReturnRef(default_screen_size_));

    protocol::MockSession* session = new MockSession();
    EXPECT_CALL(*session, jid()).WillRepeatedly(ReturnRef(client_jid_));
    EXPECT_CALL(*session, SetStateChangeCallback(_));

    client_session_ = new ClientSession(
        &session_event_handler_,
        new protocol::ConnectionToClient(
            base::MessageLoopProxy::current(), session),
         &input_stub_, &capturer_);
   }
