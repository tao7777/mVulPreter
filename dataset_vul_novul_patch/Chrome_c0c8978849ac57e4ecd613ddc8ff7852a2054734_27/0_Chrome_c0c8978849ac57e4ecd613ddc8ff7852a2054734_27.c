 void PlatformSensorProviderMac::CreateSensorInternal(
     mojom::SensorType type,
    SensorReadingSharedBuffer* reading_buffer,
     const CreateSensorCallback& callback) {
   switch (type) {
     case mojom::SensorType::AMBIENT_LIGHT: {
       scoped_refptr<PlatformSensor> sensor =
          new PlatformSensorAmbientLightMac(reading_buffer, this);
       callback.Run(std::move(sensor));
       break;
     }
     case mojom::SensorType::ACCELEROMETER: {
       callback.Run(base::MakeRefCounted<PlatformSensorAccelerometerMac>(
          reading_buffer, this));
       break;
     }
     case mojom::SensorType::RELATIVE_ORIENTATION_EULER_ANGLES: {
       auto fusion_algorithm = std::make_unique<
           RelativeOrientationEulerAnglesFusionAlgorithmUsingAccelerometer>();
      PlatformSensorFusion::Create(reading_buffer, this,
                                    std::move(fusion_algorithm), callback);
       break;
     }
    case mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION: {
      auto orientation_quaternion_fusion_algorithm_using_euler_angles =
          std::make_unique<
              OrientationQuaternionFusionAlgorithmUsingEulerAngles>(
              false /* absolute */);
       PlatformSensorFusion::Create(
          reading_buffer, this,
           std::move(orientation_quaternion_fusion_algorithm_using_euler_angles),
           callback);
       break;
    }
    default:
      callback.Run(nullptr);
  }
}
