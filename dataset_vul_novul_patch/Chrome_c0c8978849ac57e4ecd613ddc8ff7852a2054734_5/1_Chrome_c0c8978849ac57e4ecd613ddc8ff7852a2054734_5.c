 PlatformSensorAccelerometerMac::PlatformSensorAccelerometerMac(
    mojo::ScopedSharedBufferMapping mapping,
     PlatformSensorProvider* provider)
    : PlatformSensor(SensorType::ACCELEROMETER, std::move(mapping), provider),
       sudden_motion_sensor_(SuddenMotionSensor::Create()) {}
