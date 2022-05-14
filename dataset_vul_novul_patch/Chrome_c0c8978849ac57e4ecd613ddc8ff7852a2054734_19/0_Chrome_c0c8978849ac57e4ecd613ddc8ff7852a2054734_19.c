void PlatformSensorProviderBase::CreateSensor(
    mojom::SensorType type,
    const CreateSensorCallback& callback) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

  if (!CreateSharedBufferIfNeeded()) {
    callback.Run(nullptr);
     return;
   }
 
  SensorReadingSharedBuffer* reading_buffer =
      GetSensorReadingSharedBufferForType(type);
  if (!reading_buffer) {
     callback.Run(nullptr);
     return;
   }

  auto it = requests_map_.find(type);
  if (it != requests_map_.end()) {
    it->second.push_back(callback);
  } else {  // This is the first CreateSensor call.
     requests_map_[type] = CallbackQueue({callback});
 
     CreateSensorInternal(
        type, reading_buffer,
         base::Bind(&PlatformSensorProviderBase::NotifySensorCreated,
                    base::Unretained(this), type));
   }
}
