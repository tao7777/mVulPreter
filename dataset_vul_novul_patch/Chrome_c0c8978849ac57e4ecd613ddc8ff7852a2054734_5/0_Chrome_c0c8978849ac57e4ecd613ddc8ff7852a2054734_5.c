 PlatformSensorAccelerometerMac::PlatformSensorAccelerometerMac(
    SensorReadingSharedBuffer* reading_buffer,
     PlatformSensorProvider* provider)
    : PlatformSensor(SensorType::ACCELEROMETER, reading_buffer, provider),
       sudden_motion_sensor_(SuddenMotionSensor::Create()) {}
