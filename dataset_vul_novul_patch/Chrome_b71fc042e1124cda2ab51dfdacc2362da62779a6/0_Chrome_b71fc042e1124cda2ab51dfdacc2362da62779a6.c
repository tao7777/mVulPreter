bool AsyncPixelTransfersCompletedQuery::End(
    base::subtle::Atomic32 submit_count) {
  AsyncMemoryParams mem_params;
  Buffer buffer = manager()->decoder()->GetSharedMemoryBuffer(shm_id());
  if (!buffer.shared_memory)
    return false;
  mem_params.shared_memory = buffer.shared_memory;
   mem_params.shm_size = buffer.size;
   mem_params.shm_data_offset = shm_offset();
   mem_params.shm_data_size = sizeof(QuerySync);
  uint32 end = mem_params.shm_data_offset + mem_params.shm_data_size;
  if (end > mem_params.shm_size || end < mem_params.shm_data_offset)
    return false;
 
   observer_ = new AsyncPixelTransferCompletionObserverImpl(submit_count);
 
  manager()->decoder()->GetAsyncPixelTransferManager()
      ->AsyncNotifyCompletion(mem_params, observer_);

  return AddToPendingTransferQueue(submit_count);
}
