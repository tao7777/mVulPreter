DataPipeConsumerDispatcher::Deserialize(const void* data,
                                        size_t num_bytes,
                                        const ports::PortName* ports,
                                        size_t num_ports,
                                        PlatformHandle* handles,
                                        size_t num_handles) {
  if (num_ports != 1 || num_handles != 1 ||
      num_bytes != sizeof(SerializedState)) {
    return nullptr;
  }
 
   const SerializedState* state = static_cast<const SerializedState*>(data);
   if (!state->options.capacity_num_bytes || !state->options.element_num_bytes ||
      state->options.capacity_num_bytes < state->options.element_num_bytes ||
      state->read_offset >= state->options.capacity_num_bytes ||
      state->bytes_available > state->options.capacity_num_bytes) {
     return nullptr;
   }
 
  NodeController* node_controller = Core::Get()->GetNodeController();
  ports::PortRef port;
  if (node_controller->node()->GetPort(ports[0], &port) != ports::OK)
    return nullptr;

  auto region_handle = CreateSharedMemoryRegionHandleFromPlatformHandles(
      std::move(handles[0]), PlatformHandle());
  auto region = base::subtle::PlatformSharedMemoryRegion::Take(
      std::move(region_handle),
      base::subtle::PlatformSharedMemoryRegion::Mode::kUnsafe,
      state->options.capacity_num_bytes,
      base::UnguessableToken::Deserialize(state->buffer_guid_high,
                                          state->buffer_guid_low));
  auto ring_buffer =
      base::UnsafeSharedMemoryRegion::Deserialize(std::move(region));
  if (!ring_buffer.IsValid()) {
    DLOG(ERROR) << "Failed to deserialize shared buffer handle.";
    return nullptr;
  }

  scoped_refptr<DataPipeConsumerDispatcher> dispatcher =
      new DataPipeConsumerDispatcher(node_controller, port,
                                     std::move(ring_buffer), state->options,
                                     state->pipe_id);

  {
    base::AutoLock lock(dispatcher->lock_);
    dispatcher->read_offset_ = state->read_offset;
    dispatcher->bytes_available_ = state->bytes_available;
    dispatcher->new_data_available_ = state->bytes_available > 0;
     dispatcher->peer_closed_ = state->flags & kFlagPeerClosed;
     if (!dispatcher->InitializeNoLock())
       return nullptr;
    if (state->options.capacity_num_bytes >
        dispatcher->ring_buffer_mapping_.mapped_size()) {
      return nullptr;
    }
     dispatcher->UpdateSignalsStateNoLock();
   }
 
  return dispatcher;
}
