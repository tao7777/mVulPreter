 void PlatformSensor::UpdateSharedBuffer(const SensorReading& reading) {
  ReadingBuffer* buffer =
      static_cast<ReadingBuffer*>(shared_buffer_mapping_.get());
   auto& seqlock = buffer->seqlock.value();
   seqlock.WriteBegin();
   buffer->reading = reading;
  seqlock.WriteEnd();
}
