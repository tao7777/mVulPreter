PlatformSensorProviderBase::MapSharedBufferForType(mojom::SensorType type) {
  mojo::ScopedSharedBufferMapping mapping = shared_buffer_handle_->MapAtOffset(
      kReadingBufferSize, SensorReadingSharedBuffer::GetOffset(type));
  if (mapping)
    memset(mapping.get(), 0, kReadingBufferSize);
  return mapping;
 }
