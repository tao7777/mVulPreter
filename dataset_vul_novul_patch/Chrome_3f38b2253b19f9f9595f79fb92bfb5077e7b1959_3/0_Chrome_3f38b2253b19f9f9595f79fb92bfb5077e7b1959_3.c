std::unique_ptr<HistogramBase> PersistentHistogramAllocator::GetHistogram(
    Reference ref) {
  PersistentHistogramData* data =
      memory_allocator_->GetAsObject<PersistentHistogramData>(ref);
  const size_t length = memory_allocator_->GetAllocSize(ref);

  if (!data || data->name[0] == '\0' ||
      reinterpret_cast<char*>(data)[length - 1] != '\0' ||
      data->samples_metadata.id == 0 || data->logged_metadata.id == 0 ||
      (data->logged_metadata.id != data->samples_metadata.id &&
       data->logged_metadata.id != data->samples_metadata.id + 1) ||
       HashMetricName(data->name) != data->samples_metadata.id) {
     NOTREACHED();
     return nullptr;
   }
  return CreateHistogram(data);
}
