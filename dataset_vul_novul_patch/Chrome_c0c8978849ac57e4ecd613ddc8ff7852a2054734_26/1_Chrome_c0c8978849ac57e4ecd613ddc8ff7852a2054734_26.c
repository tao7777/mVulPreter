 void PlatformSensorProviderLinux::SensorDeviceFound(
     mojom::SensorType type,
    mojo::ScopedSharedBufferMapping mapping,
     const PlatformSensorProviderBase::CreateSensorCallback& callback,
     const SensorInfoLinux* sensor_device) {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(sensor_device);

  if (!StartPollingThread()) {
    callback.Run(nullptr);
    return;
   }
 
   scoped_refptr<PlatformSensorLinux> sensor =
      new PlatformSensorLinux(type, std::move(mapping), this, sensor_device,
                               polling_thread_->task_runner());
   callback.Run(sensor);
 }
