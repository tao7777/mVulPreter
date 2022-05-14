 void PlatformSensorProviderAndroid::CreateAbsoluteOrientationQuaternionSensor(
     JNIEnv* env,
    mojo::ScopedSharedBufferMapping mapping,
     const CreateSensorCallback& callback) {
   ScopedJavaLocalRef<jobject> sensor = Java_PlatformSensorProvider_createSensor(
       env, j_object_,
       static_cast<jint>(mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION));
 
   if (sensor.obj()) {
     auto concrete_sensor = base::MakeRefCounted<PlatformSensorAndroid>(
        mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION, std::move(mapping),
         this, sensor);
 
     callback.Run(concrete_sensor);
  } else {
    auto sensor_fusion_algorithm =
        std::make_unique<OrientationQuaternionFusionAlgorithmUsingEulerAngles>(
            true /* absolute */);
 
    PlatformSensorFusion::Create(std::move(mapping), this,
                                  std::move(sensor_fusion_algorithm), callback);
   }
 }
