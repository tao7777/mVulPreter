FakePlatformSensor::FakePlatformSensor(mojom::SensorType type,
FakePlatformSensor::FakePlatformSensor(
    mojom::SensorType type,
    SensorReadingSharedBuffer* reading_buffer,
    PlatformSensorProvider* provider)
    : PlatformSensor(type, reading_buffer, provider) {
   ON_CALL(*this, StartSensor(_))
       .WillByDefault(
           Invoke([this](const PlatformSensorConfiguration& configuration) {
            SensorReading reading;
            if (GetType() == mojom::SensorType::AMBIENT_LIGHT) {
              reading.als.value = configuration.frequency();
              UpdateSharedBufferAndNotifyClients(reading);
            }
            return true;
          }));
}
