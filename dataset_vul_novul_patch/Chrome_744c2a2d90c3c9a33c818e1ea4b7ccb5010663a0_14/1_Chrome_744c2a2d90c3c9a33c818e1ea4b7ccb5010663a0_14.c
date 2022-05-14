 void MockRenderThread::AddRoute(int32 routing_id,
                                 IPC::Channel::Listener* listener) {
  EXPECT_EQ(routing_id_, routing_id);
  widget_ = listener;
 }
