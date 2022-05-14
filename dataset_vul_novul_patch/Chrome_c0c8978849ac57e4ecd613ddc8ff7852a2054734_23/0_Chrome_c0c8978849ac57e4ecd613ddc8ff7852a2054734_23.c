 void PlatformSensorProviderLinux::CreateFusionSensor(
     mojom::SensorType type,
    SensorReadingSharedBuffer* reading_buffer,
     const CreateSensorCallback& callback) {
   DCHECK(IsFusionSensorType(type));
   std::unique_ptr<PlatformSensorFusionAlgorithm> fusion_algorithm;
  switch (type) {
    case mojom::SensorType::LINEAR_ACCELERATION:
      fusion_algorithm = std::make_unique<
          LinearAccelerationFusionAlgorithmUsingAccelerometer>();
      break;
    case mojom::SensorType::RELATIVE_ORIENTATION_EULER_ANGLES:
      fusion_algorithm = std::make_unique<
          RelativeOrientationEulerAnglesFusionAlgorithmUsingAccelerometer>();
      break;
    case mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION:
      fusion_algorithm = std::make_unique<
          OrientationQuaternionFusionAlgorithmUsingEulerAngles>(
          false /* absolute */);
      break;
    default:
      NOTREACHED();
   }
 
   DCHECK(fusion_algorithm);
  PlatformSensorFusion::Create(reading_buffer, this,
                                std::move(fusion_algorithm), callback);
 }
