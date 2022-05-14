 void PlatformSensorProviderAndroid::CreateSensorInternal(
     mojom::SensorType type,
    SensorReadingSharedBuffer* reading_buffer,
     const CreateSensorCallback& callback) {
   JNIEnv* env = AttachCurrentThread();
 
   switch (type) {
     case mojom::SensorType::ABSOLUTE_ORIENTATION_EULER_ANGLES:
      CreateAbsoluteOrientationEulerAnglesSensor(env, reading_buffer, callback);
       break;
     case mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION:
      CreateAbsoluteOrientationQuaternionSensor(env, reading_buffer, callback);
       break;
     case mojom::SensorType::RELATIVE_ORIENTATION_EULER_ANGLES:
      CreateRelativeOrientationEulerAnglesSensor(env, reading_buffer, callback);
       break;
     default: {
       ScopedJavaLocalRef<jobject> sensor =
          Java_PlatformSensorProvider_createSensor(env, j_object_,
                                                   static_cast<jint>(type));

      if (!sensor.obj()) {
        callback.Run(nullptr);
        return;
       }
 
       auto concrete_sensor = base::MakeRefCounted<PlatformSensorAndroid>(
          type, reading_buffer, this, sensor);
       callback.Run(concrete_sensor);
       break;
     }
  }
}
