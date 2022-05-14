 void PlatformSensorProviderAndroid::CreateRelativeOrientationEulerAnglesSensor(
     JNIEnv* env,
    mojo::ScopedSharedBufferMapping mapping,
     const CreateSensorCallback& callback) {
   if (static_cast<bool>(Java_PlatformSensorProvider_hasSensorType(
           env, j_object_,
          static_cast<jint>(
              mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION)))) {
    auto sensor_fusion_algorithm =
        std::make_unique<OrientationEulerAnglesFusionAlgorithmUsingQuaternion>(
            false /* absolute */);
 
    PlatformSensorFusion::Create(std::move(mapping), this,
                                  std::move(sensor_fusion_algorithm), callback);
   } else {
     callback.Run(nullptr);
  }
}
