mojo::ScopedSharedBufferMapping FakePlatformSensorProvider::GetMapping(
     mojom::SensorType type) {
  return CreateSharedBufferIfNeeded() ? MapSharedBufferForType(type) : nullptr;
 }
