 void PlatformSensorFusion::Create(
    mojo::ScopedSharedBufferMapping mapping,
     PlatformSensorProvider* provider,
     std::unique_ptr<PlatformSensorFusionAlgorithm> fusion_algorithm,
     const PlatformSensorProviderBase::CreateSensorCallback& callback) {
  Factory::CreateSensorFusion(std::move(mapping), std::move(fusion_algorithm),
                               callback, provider);
 }
