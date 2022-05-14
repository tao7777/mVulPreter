 void SpdyWriteQueue::Clear() {
   CHECK(!removing_writes_);
   removing_writes_ = true;
  std::vector<SpdyBufferProducer*> erased_buffer_producers;

   for (int i = MINIMUM_PRIORITY; i <= MAXIMUM_PRIORITY; ++i) {
     for (std::deque<PendingWrite>::iterator it = queue_[i].begin();
          it != queue_[i].end(); ++it) {
      erased_buffer_producers.push_back(it->frame_producer);
     }
     queue_[i].clear();
   }
   removing_writes_ = false;
  STLDeleteElements(&erased_buffer_producers);  // Invokes callbacks.
 }
