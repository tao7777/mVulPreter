 void PlatformSensorProviderAndroid::CreateAbsoluteOrientationEulerAnglesSensor(
     JNIEnv* env,
    SensorReadingSharedBuffer* reading_buffer,
     const CreateSensorCallback& callback) {
   if (static_cast<bool>(Java_PlatformSensorProvider_hasSensorType(
           env, j_object_,
          static_cast<jint>(
              mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION)))) {
    auto sensor_fusion_algorithm =
        std::make_unique<OrientationEulerAnglesFusionAlgorithmUsingQuaternion>(
            true /* absolute */);
 
    PlatformSensorFusion::Create(reading_buffer, this,
                                  std::move(sensor_fusion_algorithm), callback);
   } else {
     auto sensor_fusion_algorithm = std::make_unique<
         AbsoluteOrientationEulerAnglesFusionAlgorithmUsingAccelerometerAndMagnetometer>();
 
    PlatformSensorFusion::Create(reading_buffer, this,
                                  std::move(sensor_fusion_algorithm), callback);
   }
 }
