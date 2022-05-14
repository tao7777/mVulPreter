 PlatformSensorAmbientLightMac::PlatformSensorAmbientLightMac(
    SensorReadingSharedBuffer* reading_buffer,
     PlatformSensorProvider* provider)
    : PlatformSensor(SensorType::AMBIENT_LIGHT, reading_buffer, provider),
       light_sensor_port_(nullptr),
       current_lux_(0.0) {}
