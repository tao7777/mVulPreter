 PlatformSensorAmbientLightMac::PlatformSensorAmbientLightMac(
    mojo::ScopedSharedBufferMapping mapping,
     PlatformSensorProvider* provider)
    : PlatformSensor(SensorType::AMBIENT_LIGHT, std::move(mapping), provider),
       light_sensor_port_(nullptr),
       current_lux_(0.0) {}
