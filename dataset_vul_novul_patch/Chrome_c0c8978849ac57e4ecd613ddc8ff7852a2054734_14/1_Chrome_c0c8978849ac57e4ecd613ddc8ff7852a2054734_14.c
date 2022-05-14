 PlatformSensorLinux::PlatformSensorLinux(
     mojom::SensorType type,
    mojo::ScopedSharedBufferMapping mapping,
     PlatformSensorProvider* provider,
     const SensorInfoLinux* sensor_device,
     scoped_refptr<base::SingleThreadTaskRunner> polling_thread_task_runner)
    : PlatformSensor(type, std::move(mapping), provider),
       default_configuration_(
           PlatformSensorConfiguration(sensor_device->device_frequency)),
       reporting_mode_(sensor_device->reporting_mode),
      polling_thread_task_runner_(std::move(polling_thread_task_runner)),
      weak_factory_(this) {
  sensor_reader_ = SensorReader::Create(
      sensor_device, weak_factory_.GetWeakPtr(), task_runner_);
}
