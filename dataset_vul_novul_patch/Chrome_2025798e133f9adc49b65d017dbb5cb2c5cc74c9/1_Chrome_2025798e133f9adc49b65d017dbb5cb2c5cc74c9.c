 void SpdyWriteQueue::Clear() {
   CHECK(!removing_writes_);
   removing_writes_ = true;
   for (int i = MINIMUM_PRIORITY; i <= MAXIMUM_PRIORITY; ++i) {
     for (std::deque<PendingWrite>::iterator it = queue_[i].begin();
          it != queue_[i].end(); ++it) {
      delete it->frame_producer;
     }
     queue_[i].clear();
   }
   removing_writes_ = false;
 }
