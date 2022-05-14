bool SoftwareFrameManager::SwapToNewFrame(
    uint32 output_surface_id,
    const cc::SoftwareFrameData* frame_data,
    float frame_device_scale_factor,
    base::ProcessHandle process_handle) {

#ifdef OS_WIN
  scoped_ptr<base::SharedMemory> shared_memory(
      new base::SharedMemory(frame_data->handle, true,
                             process_handle));
#else
  scoped_ptr<base::SharedMemory> shared_memory(
      new base::SharedMemory(frame_data->handle, true));
#endif
 
   if (base::SharedMemory::IsHandleValid(shared_memory->handle())) {
    const size_t size_in_bytes = 4 * frame_data->size.GetArea();
 #ifdef OS_WIN
     if (!shared_memory->Map(0)) {
       DLOG(ERROR) << "Unable to map renderer memory.";
      RecordAction(
          base::UserMetricsAction("BadMessageTerminate_SharedMemoryManager1"));
      return false;
    }

    if (shared_memory->mapped_size() < size_in_bytes) {
      DLOG(ERROR) << "Shared memory too small for given rectangle";
      RecordAction(
          base::UserMetricsAction("BadMessageTerminate_SharedMemoryManager2"));
      return false;
    }
#else
    if (!shared_memory->Map(size_in_bytes)) {
      DLOG(ERROR) << "Unable to map renderer memory.";
      RecordAction(
          base::UserMetricsAction("BadMessageTerminate_SharedMemoryManager1"));
      return false;
    }
#endif
  }

  scoped_refptr<SoftwareFrame> next_frame(new SoftwareFrame(
      client_,
      output_surface_id,
      frame_data->id,
      frame_device_scale_factor,
      frame_data->size,
      shared_memory.Pass()));
  current_frame_.swap(next_frame);
  return true;
}
