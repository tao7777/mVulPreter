 void MockRenderThread::RemoveRoute(int32 routing_id) {
  EXPECT_EQ(routing_id_, routing_id);
  widget_ = NULL;
 }
