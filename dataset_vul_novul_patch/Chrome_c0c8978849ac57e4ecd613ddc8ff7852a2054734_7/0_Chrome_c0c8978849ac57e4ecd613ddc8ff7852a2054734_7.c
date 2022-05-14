 PlatformSensorAndroid::PlatformSensorAndroid(
     mojom::SensorType type,
    SensorReadingSharedBuffer* reading_buffer,
     PlatformSensorProvider* provider,
     const JavaRef<jobject>& java_sensor)
    : PlatformSensor(type, reading_buffer, provider) {
   JNIEnv* env = AttachCurrentThread();
   j_object_.Reset(java_sensor);
 
  Java_PlatformSensor_initPlatformSensorAndroid(env, j_object_,
                                                reinterpret_cast<jlong>(this));
}
