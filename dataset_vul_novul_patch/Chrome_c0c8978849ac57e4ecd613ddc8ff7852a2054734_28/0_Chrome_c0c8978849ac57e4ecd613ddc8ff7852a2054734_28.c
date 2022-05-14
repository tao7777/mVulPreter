 void PlatformSensorProviderWin::CreateSensorInternal(
     mojom::SensorType type,
    SensorReadingSharedBuffer* reading_buffer,
     const CreateSensorCallback& callback) {
   DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
   if (!StartSensorThread()) {
    callback.Run(nullptr);
    return;
  }

  switch (type) {
    case mojom::SensorType::LINEAR_ACCELERATION: {
      auto linear_acceleration_fusion_algorithm = std::make_unique<
          LinearAccelerationFusionAlgorithmUsingAccelerometer>();
       PlatformSensorFusion::Create(
          reading_buffer, this, std::move(linear_acceleration_fusion_algorithm),
          callback);
       break;
     }
 
    default: {
      base::PostTaskAndReplyWithResult(
          sensor_thread_->task_runner().get(), FROM_HERE,
           base::Bind(&PlatformSensorProviderWin::CreateSensorReader,
                      base::Unretained(this), type),
           base::Bind(&PlatformSensorProviderWin::SensorReaderCreated,
                     base::Unretained(this), type, reading_buffer, callback));
       break;
     }
   }
}
