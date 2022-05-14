  void SensorCreated(scoped_refptr<PlatformSensor> sensor) {
    if (!result_callback_) {
      return;
    }

    if (!sensor) {
      std::move(result_callback_).Run(nullptr);
      return;
    }
    mojom::SensorType type = sensor->GetType();
     sources_map_[type] = std::move(sensor);
     if (sources_map_.size() == fusion_algorithm_->source_types().size()) {
       scoped_refptr<PlatformSensor> fusion_sensor(new PlatformSensorFusion(
          std::move(mapping_), provider_, std::move(fusion_algorithm_),
           std::move(sources_map_)));
       std::move(result_callback_).Run(fusion_sensor);
     }
   }
