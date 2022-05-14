 void PlatformSensorProviderMac::CreateSensorInternal(
     mojom::SensorType type,
    mojo::ScopedSharedBufferMapping mapping,
     const CreateSensorCallback& callback) {
   switch (type) {
     case mojom::SensorType::AMBIENT_LIGHT: {
       scoped_refptr<PlatformSensor> sensor =
          new PlatformSensorAmbientLightMac(std::move(mapping), this);
       callback.Run(std::move(sensor));
       break;
     }
     case mojom::SensorType::ACCELEROMETER: {
       callback.Run(base::MakeRefCounted<PlatformSensorAccelerometerMac>(
          std::move(mapping), this));
       break;
     }
     case mojom::SensorType::RELATIVE_ORIENTATION_EULER_ANGLES: {
       auto fusion_algorithm = std::make_unique<
           RelativeOrientationEulerAnglesFusionAlgorithmUsingAccelerometer>();
      PlatformSensorFusion::Create(std::move(mapping), this,
                                    std::move(fusion_algorithm), callback);
       break;
     }
    case mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION: {
      auto orientation_quaternion_fusion_algorithm_using_euler_angles =
          std::make_unique<
              OrientationQuaternionFusionAlgorithmUsingEulerAngles>(
              false /* absolute */);
       PlatformSensorFusion::Create(
          std::move(mapping), this,
           std::move(orientation_quaternion_fusion_algorithm_using_euler_angles),
           callback);
       break;
    }
    default:
      callback.Run(nullptr);
  }
}
