 void SpdyWriteQueue::RemovePendingWritesForStreamsAfter(
     SpdyStreamId last_good_stream_id) {
   CHECK(!removing_writes_);
   removing_writes_ = true;
   for (int i = MINIMUM_PRIORITY; i <= MAXIMUM_PRIORITY; ++i) {
     std::deque<PendingWrite>* queue = &queue_[i];
    std::deque<PendingWrite>::iterator out_it = queue->begin();
    for (std::deque<PendingWrite>::const_iterator it = queue->begin();
          it != queue->end(); ++it) {
       if (it->stream.get() && (it->stream->stream_id() > last_good_stream_id ||
                                it->stream->stream_id() == 0)) {
        delete it->frame_producer;
       } else {
         *out_it = *it;
         ++out_it;
      }
    }
     queue->erase(out_it, queue->end());
   }
   removing_writes_ = false;
 }
