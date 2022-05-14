 void PlatformSensor::UpdateSharedBuffer(const SensorReading& reading) {
  ReadingBuffer* buffer = reading_buffer_;
   auto& seqlock = buffer->seqlock.value();
   seqlock.WriteBegin();
   buffer->reading = reading;
  seqlock.WriteEnd();
}
