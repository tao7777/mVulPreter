PlatformSensorProviderBase::MapSharedBufferForType(mojom::SensorType type) {
SensorReadingSharedBuffer*
PlatformSensorProviderBase::GetSensorReadingSharedBufferForType(
    mojom::SensorType type) {
  auto* ptr = static_cast<char*>(shared_buffer_mapping_.get());
  if (!ptr)
    return nullptr;

  ptr += SensorReadingSharedBuffer::GetOffset(type);
  memset(ptr, 0, kReadingBufferSize);
  return reinterpret_cast<SensorReadingSharedBuffer*>(ptr);
 }
