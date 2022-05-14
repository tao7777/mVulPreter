 PlatformSensor::PlatformSensor(mojom::SensorType type,
                               mojo::ScopedSharedBufferMapping mapping,
                                PlatformSensorProvider* provider)
     : task_runner_(base::ThreadTaskRunnerHandle::Get()),
      shared_buffer_mapping_(std::move(mapping)),
       type_(type),
       provider_(provider),
       weak_factory_(this) {}
