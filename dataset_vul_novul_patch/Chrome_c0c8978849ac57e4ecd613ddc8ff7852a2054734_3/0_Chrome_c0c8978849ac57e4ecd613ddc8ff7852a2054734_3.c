 PlatformSensor::PlatformSensor(mojom::SensorType type,
                               SensorReadingSharedBuffer* reading_buffer,
                                PlatformSensorProvider* provider)
     : task_runner_(base::ThreadTaskRunnerHandle::Get()),
      reading_buffer_(reading_buffer),
       type_(type),
       provider_(provider),
       weak_factory_(this) {}
