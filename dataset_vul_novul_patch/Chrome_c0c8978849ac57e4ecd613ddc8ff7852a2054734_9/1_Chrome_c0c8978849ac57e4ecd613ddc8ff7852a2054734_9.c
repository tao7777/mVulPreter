   static void CreateSensorFusion(
      mojo::ScopedSharedBufferMapping mapping,
       std::unique_ptr<PlatformSensorFusionAlgorithm> fusion_algorithm,
       const PlatformSensorProviderBase::CreateSensorCallback& callback,
       PlatformSensorProvider* provider) {
    scoped_refptr<Factory> factory(new Factory(std::move(mapping),
                                                std::move(fusion_algorithm),
                                                std::move(callback), provider));
     factory->FetchSources();
  }
