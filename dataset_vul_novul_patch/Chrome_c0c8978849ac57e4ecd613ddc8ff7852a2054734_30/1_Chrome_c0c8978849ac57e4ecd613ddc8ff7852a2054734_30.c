 PlatformSensorWin::PlatformSensorWin(
     mojom::SensorType type,
    mojo::ScopedSharedBufferMapping mapping,
     PlatformSensorProvider* provider,
     scoped_refptr<base::SingleThreadTaskRunner> sensor_thread_runner,
     std::unique_ptr<PlatformSensorReaderWin> sensor_reader)
    : PlatformSensor(type, std::move(mapping), provider),
       sensor_thread_runner_(sensor_thread_runner),
       sensor_reader_(sensor_reader.release()),
       weak_factory_(this) {
  DCHECK(sensor_reader_);
  sensor_reader_->SetClient(this);
}
