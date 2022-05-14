 bool PlatformSensor::GetLatestReading(SensorReading* result) {
   if (!shared_buffer_reader_) {
    const auto* buffer = static_cast<const device::SensorReadingSharedBuffer*>(
        shared_buffer_mapping_.get());
     shared_buffer_reader_ =
        std::make_unique<SensorReadingSharedBufferReader>(buffer);
   }
 
   return shared_buffer_reader_->GetReading(result);
}
