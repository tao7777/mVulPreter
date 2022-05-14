 bool PlatformSensor::GetLatestReading(SensorReading* result) {
   if (!shared_buffer_reader_) {
     shared_buffer_reader_ =
        std::make_unique<SensorReadingSharedBufferReader>(reading_buffer_);
   }
 
   return shared_buffer_reader_->GetReading(result);
}
