  void CreateFusionSensor(
      std::unique_ptr<PlatformSensorFusionAlgorithm> fusion_algorithm) {
    auto callback =
         base::Bind(&PlatformSensorFusionTest::PlatformSensorFusionCallback,
                    base::Unretained(this));
     SensorType type = fusion_algorithm->fused_type();
    PlatformSensorFusion::Create(provider_->GetSensorReadingBuffer(type),
                                 provider_.get(), std::move(fusion_algorithm),
                                 callback);
     EXPECT_TRUE(platform_sensor_fusion_callback_called_);
   }
