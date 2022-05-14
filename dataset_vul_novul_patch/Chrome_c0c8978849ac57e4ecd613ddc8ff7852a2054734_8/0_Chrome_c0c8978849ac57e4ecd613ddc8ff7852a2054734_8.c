 void PlatformSensorFusion::Create(
    SensorReadingSharedBuffer* reading_buffer,
     PlatformSensorProvider* provider,
     std::unique_ptr<PlatformSensorFusionAlgorithm> fusion_algorithm,
     const PlatformSensorProviderBase::CreateSensorCallback& callback) {
  Factory::CreateSensorFusion(reading_buffer, std::move(fusion_algorithm),
                               callback, provider);
 }
