  Factory(mojo::ScopedSharedBufferMapping mapping,
  Factory(SensorReadingSharedBuffer* reading_buffer,
           std::unique_ptr<PlatformSensorFusionAlgorithm> fusion_algorithm,
           const PlatformSensorProviderBase::CreateSensorCallback& callback,
           PlatformSensorProvider* provider)
       : fusion_algorithm_(std::move(fusion_algorithm)),
         result_callback_(std::move(callback)),
        reading_buffer_(reading_buffer),
         provider_(provider) {
     const auto& types = fusion_algorithm_->source_types();
     DCHECK(!types.empty());
     DCHECK(std::adjacent_find(types.begin(), types.end()) == types.end());
     DCHECK(result_callback_);
    DCHECK(reading_buffer_);
     DCHECK(provider_);
   }
