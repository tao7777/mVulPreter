mojo::ScopedSharedBufferMapping FakePlatformSensorProvider::GetMapping(
SensorReadingSharedBuffer* FakePlatformSensorProvider::GetSensorReadingBuffer(
     mojom::SensorType type) {
  return CreateSharedBufferIfNeeded()
             ? GetSensorReadingSharedBufferForType(type)
             : nullptr;
 }
