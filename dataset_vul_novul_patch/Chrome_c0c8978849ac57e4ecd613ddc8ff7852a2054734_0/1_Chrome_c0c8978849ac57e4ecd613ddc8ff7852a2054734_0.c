FakePlatformSensor::FakePlatformSensor(mojom::SensorType type,
                                       mojo::ScopedSharedBufferMapping mapping,
                                       PlatformSensorProvider* provider)
    : PlatformSensor(type, std::move(mapping), provider) {
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
