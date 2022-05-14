   static void CreateSensorFusion(
      SensorReadingSharedBuffer* reading_buffer,
       std::unique_ptr<PlatformSensorFusionAlgorithm> fusion_algorithm,
       const PlatformSensorProviderBase::CreateSensorCallback& callback,
       PlatformSensorProvider* provider) {
    scoped_refptr<Factory> factory(new Factory(reading_buffer,
                                                std::move(fusion_algorithm),
                                                std::move(callback), provider));
     factory->FetchSources();
  }
