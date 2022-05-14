 void PlatformSensorProviderWin::SensorReaderCreated(
     mojom::SensorType type,
    SensorReadingSharedBuffer* reading_buffer,
     const CreateSensorCallback& callback,
     std::unique_ptr<PlatformSensorReaderWin> sensor_reader) {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  if (!sensor_reader) {
    callback.Run(nullptr);
    return;
   }
 
   scoped_refptr<PlatformSensor> sensor = new PlatformSensorWin(
      type, reading_buffer, this, sensor_thread_->task_runner(),
       std::move(sensor_reader));
   callback.Run(sensor);
 }
