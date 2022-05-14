 void MockRenderThread::RemoveRoute(int32 routing_id) {
  // We may hear this for views created from OnMsgCreateWindow as well,
  // in which case we don't want to track the new widget.
  if (routing_id_ == routing_id)
    widget_ = NULL;
 }
