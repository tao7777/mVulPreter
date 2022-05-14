 void FakePlatformSensorProvider::CreateSensorInternal(
     mojom::SensorType type,
    mojo::ScopedSharedBufferMapping mapping,
     const CreateSensorCallback& callback) {
   DCHECK(type >= mojom::SensorType::FIRST && type <= mojom::SensorType::LAST);
   auto sensor =
      base::MakeRefCounted<FakePlatformSensor>(type, std::move(mapping), this);
   DoCreateSensorInternal(type, std::move(sensor), callback);
 }
