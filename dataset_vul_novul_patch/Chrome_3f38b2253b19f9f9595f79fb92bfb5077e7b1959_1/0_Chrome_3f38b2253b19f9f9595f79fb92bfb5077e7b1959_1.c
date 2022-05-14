 std::unique_ptr<HistogramBase> PersistentHistogramAllocator::CreateHistogram(
     PersistentHistogramData* histogram_data_ptr) {
   if (!histogram_data_ptr) {
     NOTREACHED();
     return nullptr;
   }

  if (histogram_data_ptr->histogram_type == SPARSE_HISTOGRAM) {
    std::unique_ptr<HistogramBase> histogram =
        SparseHistogram::PersistentCreate(this, histogram_data_ptr->name,
                                          &histogram_data_ptr->samples_metadata,
                                           &histogram_data_ptr->logged_metadata);
     DCHECK(histogram);
     histogram->SetFlags(histogram_data_ptr->flags);
     return histogram;
   }
 
  int32_t histogram_type = histogram_data_ptr->histogram_type;
  int32_t histogram_flags = histogram_data_ptr->flags;
  int32_t histogram_minimum = histogram_data_ptr->minimum;
  int32_t histogram_maximum = histogram_data_ptr->maximum;
  uint32_t histogram_bucket_count = histogram_data_ptr->bucket_count;
  uint32_t histogram_ranges_ref = histogram_data_ptr->ranges_ref;
  uint32_t histogram_ranges_checksum = histogram_data_ptr->ranges_checksum;

  HistogramBase::Sample* ranges_data =
      memory_allocator_->GetAsArray<HistogramBase::Sample>(
          histogram_ranges_ref, kTypeIdRangesArray,
          PersistentMemoryAllocator::kSizeAny);

  const uint32_t max_buckets =
      std::numeric_limits<uint32_t>::max() / sizeof(HistogramBase::Sample);
  size_t required_bytes =
      (histogram_bucket_count + 1) * sizeof(HistogramBase::Sample);
  size_t allocated_bytes =
      memory_allocator_->GetAllocSize(histogram_ranges_ref);
   if (!ranges_data || histogram_bucket_count < 2 ||
       histogram_bucket_count >= max_buckets ||
       allocated_bytes < required_bytes) {
     NOTREACHED();
     return nullptr;
   }
 
   std::unique_ptr<const BucketRanges> created_ranges = CreateRangesFromData(
       ranges_data, histogram_ranges_checksum, histogram_bucket_count + 1);
   if (!created_ranges) {
     NOTREACHED();
     return nullptr;
   }
  const BucketRanges* ranges =
      StatisticsRecorder::RegisterOrDeleteDuplicateRanges(
          created_ranges.release());

  size_t counts_bytes = CalculateRequiredCountsBytes(histogram_bucket_count);
  PersistentMemoryAllocator::Reference counts_ref =
      subtle::Acquire_Load(&histogram_data_ptr->counts_ref);
   if (counts_bytes == 0 ||
       (counts_ref != 0 &&
        memory_allocator_->GetAllocSize(counts_ref) < counts_bytes)) {
     NOTREACHED();
     return nullptr;
   }

  DelayedPersistentAllocation counts_data(memory_allocator_.get(),
                                          &histogram_data_ptr->counts_ref,
                                          kTypeIdCountsArray, counts_bytes, 0);

  DelayedPersistentAllocation logged_data(
      memory_allocator_.get(), &histogram_data_ptr->counts_ref,
      kTypeIdCountsArray, counts_bytes, counts_bytes / 2,
      /*make_iterable=*/false);

  const char* name = histogram_data_ptr->name;
  std::unique_ptr<HistogramBase> histogram;
  switch (histogram_type) {
    case HISTOGRAM:
      histogram = Histogram::PersistentCreate(
          name, histogram_minimum, histogram_maximum, ranges, counts_data,
          logged_data, &histogram_data_ptr->samples_metadata,
          &histogram_data_ptr->logged_metadata);
      DCHECK(histogram);
      break;
    case LINEAR_HISTOGRAM:
      histogram = LinearHistogram::PersistentCreate(
          name, histogram_minimum, histogram_maximum, ranges, counts_data,
          logged_data, &histogram_data_ptr->samples_metadata,
          &histogram_data_ptr->logged_metadata);
      DCHECK(histogram);
      break;
    case BOOLEAN_HISTOGRAM:
      histogram = BooleanHistogram::PersistentCreate(
          name, ranges, counts_data, logged_data,
          &histogram_data_ptr->samples_metadata,
          &histogram_data_ptr->logged_metadata);
      DCHECK(histogram);
      break;
    case CUSTOM_HISTOGRAM:
      histogram = CustomHistogram::PersistentCreate(
          name, ranges, counts_data, logged_data,
          &histogram_data_ptr->samples_metadata,
          &histogram_data_ptr->logged_metadata);
      DCHECK(histogram);
      break;
    default:
      NOTREACHED();
  }

   if (histogram) {
     DCHECK_EQ(histogram_type, histogram->GetHistogramType());
     histogram->SetFlags(histogram_flags);
   }
 
   return histogram;
}
