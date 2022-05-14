    CancelPendingTask() {
  filter_->ResumeAttachOrDestroy(element_instance_id_,
                                 MSG_ROUTING_NONE /* no plugin frame */);
}
