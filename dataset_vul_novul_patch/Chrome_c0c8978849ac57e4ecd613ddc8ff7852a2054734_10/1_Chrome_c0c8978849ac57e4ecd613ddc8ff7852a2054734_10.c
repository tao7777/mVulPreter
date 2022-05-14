  Factory(mojo::ScopedSharedBufferMapping mapping,
           std::unique_ptr<PlatformSensorFusionAlgorithm> fusion_algorithm,
           const PlatformSensorProviderBase::CreateSensorCallback& callback,
           PlatformSensorProvider* provider)
       : fusion_algorithm_(std::move(fusion_algorithm)),
         result_callback_(std::move(callback)),
        mapping_(std::move(mapping)),
         provider_(provider) {
     const auto& types = fusion_algorithm_->source_types();
     DCHECK(!types.empty());
     DCHECK(std::adjacent_find(types.begin(), types.end()) == types.end());
     DCHECK(result_callback_);
    DCHECK(mapping_);
     DCHECK(provider_);
   }
