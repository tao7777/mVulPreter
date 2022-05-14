 PlatformSensorFusion::PlatformSensorFusion(
    mojo::ScopedSharedBufferMapping mapping,
     PlatformSensorProvider* provider,
     std::unique_ptr<PlatformSensorFusionAlgorithm> fusion_algorithm,
     PlatformSensorFusion::SourcesMap sources)
    : PlatformSensor(fusion_algorithm->fused_type(),
                     std::move(mapping),
                     provider),
       fusion_algorithm_(std::move(fusion_algorithm)),
       source_sensors_(std::move(sources)),
       reporting_mode_(mojom::ReportingMode::CONTINUOUS) {
  for (const auto& pair : source_sensors_)
    pair.second->AddClient(this);

  fusion_algorithm_->set_fusion_sensor(this);

  if (std::any_of(source_sensors_.begin(), source_sensors_.end(),
                  [](const SourcesMapEntry& pair) {
                    return pair.second->GetReportingMode() ==
                           mojom::ReportingMode::ON_CHANGE;
                  })) {
    reporting_mode_ = mojom::ReportingMode::ON_CHANGE;
  }
}
