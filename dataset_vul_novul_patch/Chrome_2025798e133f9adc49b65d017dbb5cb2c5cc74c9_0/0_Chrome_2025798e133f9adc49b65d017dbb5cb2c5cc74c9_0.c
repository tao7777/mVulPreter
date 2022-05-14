void SpdyWriteQueue::RemovePendingWritesForStream(
    const base::WeakPtr<SpdyStream>& stream) {
  CHECK(!removing_writes_);
  removing_writes_ = true;
  RequestPriority priority = stream->priority();
  CHECK_GE(priority, MINIMUM_PRIORITY);
  CHECK_LE(priority, MAXIMUM_PRIORITY);

  DCHECK(stream.get());
#if DCHECK_IS_ON
  for (int i = MINIMUM_PRIORITY; i <= MAXIMUM_PRIORITY; ++i) {
    if (priority == i)
      continue;
    for (std::deque<PendingWrite>::const_iterator it = queue_[i].begin();
         it != queue_[i].end(); ++it) {
      DCHECK_NE(it->stream.get(), stream.get());
    }
   }
 #endif
 
  // Defer deletion until queue iteration is complete, as
  // SpdyBuffer::~SpdyBuffer() can result in callbacks into SpdyWriteQueue.
  std::vector<SpdyBufferProducer*> erased_buffer_producers;

   std::deque<PendingWrite>* queue = &queue_[priority];
   std::deque<PendingWrite>::iterator out_it = queue->begin();
   for (std::deque<PendingWrite>::const_iterator it = queue->begin();
        it != queue->end(); ++it) {
     if (it->stream.get() == stream.get()) {
      erased_buffer_producers.push_back(it->frame_producer);
     } else {
       *out_it = *it;
       ++out_it;
     }
   }
   queue->erase(out_it, queue->end());
   removing_writes_ = false;
  STLDeleteElements(&erased_buffer_producers);  // Invokes callbacks.
 }
