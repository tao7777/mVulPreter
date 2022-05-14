 void PlatformSensorProviderLinux::CreateSensorInternal(
     mojom::SensorType type,
    mojo::ScopedSharedBufferMapping mapping,
     const CreateSensorCallback& callback) {
   if (!sensor_device_manager_)
     sensor_device_manager_.reset(new SensorDeviceManager());
 
   if (IsFusionSensorType(type)) {
    CreateFusionSensor(type, std::move(mapping), callback);
     return;
   }
 
  if (!sensor_nodes_enumerated_) {
    if (!sensor_nodes_enumeration_started_) {
      sensor_nodes_enumeration_started_ = file_task_runner_->PostTask(
          FROM_HERE,
          base::Bind(&SensorDeviceManager::Start,
                     base::Unretained(sensor_device_manager_.get()), this));
    }
    return;
  }

  SensorInfoLinux* sensor_device = GetSensorDevice(type);
  if (!sensor_device) {
    callback.Run(nullptr);
     return;
   }
 
  SensorDeviceFound(type, std::move(mapping), callback, sensor_device);
 }
