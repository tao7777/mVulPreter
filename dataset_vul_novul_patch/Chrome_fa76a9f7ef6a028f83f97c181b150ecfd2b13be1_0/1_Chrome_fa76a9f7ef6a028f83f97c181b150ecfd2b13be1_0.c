void CoordinatorImpl::FinalizeGlobalMemoryDumpIfAllManagersReplied() {
  TRACE_EVENT0(base::trace_event::MemoryDumpManager::kTraceCategory,
               "GlobalMemoryDump.Computation");
  DCHECK(!queued_memory_dump_requests_.empty());

  QueuedRequest* request = &queued_memory_dump_requests_.front();
  if (!request->dump_in_progress || request->pending_responses.size() > 0 ||
      request->heap_dump_in_progress) {
    return;
  }

  QueuedRequestDispatcher::Finalize(request, tracing_observer_.get());

  queued_memory_dump_requests_.pop_front();
  request = nullptr;

  if (!queued_memory_dump_requests_.empty()) {
     base::SequencedTaskRunnerHandle::Get()->PostTask(
         FROM_HERE,
         base::BindOnce(&CoordinatorImpl::PerformNextQueuedGlobalMemoryDump,
                       base::Unretained(this)));
   }
 }
