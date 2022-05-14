std::unique_ptr<HistogramBase> PersistentHistogramAllocator::AllocateHistogram(
    HistogramType histogram_type,
    const std::string& name,
    int minimum,
    int maximum,
    const BucketRanges* bucket_ranges,
    int32_t flags,
    Reference* ref_ptr) {
  if (memory_allocator_->IsCorrupt()) {
    RecordCreateHistogramResult(CREATE_HISTOGRAM_ALLOCATOR_CORRUPT);
     return nullptr;
  }
 
  PersistentHistogramData* histogram_data =
      memory_allocator_->New<PersistentHistogramData>(
          offsetof(PersistentHistogramData, name) + name.length() + 1);
  if (histogram_data) {
    memcpy(histogram_data->name, name.c_str(), name.size() + 1);
    histogram_data->histogram_type = histogram_type;
    histogram_data->flags = flags | HistogramBase::kIsPersistent;
  }

  if (histogram_type != SPARSE_HISTOGRAM) {
    size_t bucket_count = bucket_ranges->bucket_count();
    size_t counts_bytes = CalculateRequiredCountsBytes(bucket_count);
    if (counts_bytes == 0) {
      NOTREACHED();
      return nullptr;
    }

    DCHECK_EQ(this, GlobalHistogramAllocator::Get());

    PersistentMemoryAllocator::Reference ranges_ref =
        bucket_ranges->persistent_reference();
    if (!ranges_ref) {
      size_t ranges_count = bucket_count + 1;
      size_t ranges_bytes = ranges_count * sizeof(HistogramBase::Sample);
      ranges_ref =
          memory_allocator_->Allocate(ranges_bytes, kTypeIdRangesArray);
      if (ranges_ref) {
        HistogramBase::Sample* ranges_data =
            memory_allocator_->GetAsArray<HistogramBase::Sample>(
                ranges_ref, kTypeIdRangesArray, ranges_count);
        if (ranges_data) {
          for (size_t i = 0; i < bucket_ranges->size(); ++i)
            ranges_data[i] = bucket_ranges->range(i);
          bucket_ranges->set_persistent_reference(ranges_ref);
        } else {
          NOTREACHED();
          ranges_ref = PersistentMemoryAllocator::kReferenceNull;
        }
      }
    } else {
      DCHECK_EQ(kTypeIdRangesArray, memory_allocator_->GetType(ranges_ref));
    }


    if (ranges_ref && histogram_data) {
      histogram_data->minimum = minimum;
      histogram_data->maximum = maximum;
      histogram_data->bucket_count = static_cast<uint32_t>(bucket_count);
      histogram_data->ranges_ref = ranges_ref;
      histogram_data->ranges_checksum = bucket_ranges->checksum();
    } else {
      histogram_data = nullptr;  // Clear this for proper handling below.
    }
  }

  if (histogram_data) {
    std::unique_ptr<HistogramBase> histogram = CreateHistogram(histogram_data);
    DCHECK(histogram);
    DCHECK_NE(0U, histogram_data->samples_metadata.id);
    DCHECK_NE(0U, histogram_data->logged_metadata.id);

    PersistentMemoryAllocator::Reference histogram_ref =
        memory_allocator_->GetAsReference(histogram_data);
    if (ref_ptr != nullptr)
      *ref_ptr = histogram_ref;

    subtle::NoBarrier_Store(&last_created_, histogram_ref);
     return histogram;
   }
 
  CreateHistogramResultType result;
  if (memory_allocator_->IsCorrupt()) {
    RecordCreateHistogramResult(CREATE_HISTOGRAM_ALLOCATOR_NEWLY_CORRUPT);
    result = CREATE_HISTOGRAM_ALLOCATOR_CORRUPT;
  } else if (memory_allocator_->IsFull()) {
    result = CREATE_HISTOGRAM_ALLOCATOR_FULL;
  } else {
    result = CREATE_HISTOGRAM_ALLOCATOR_ERROR;
  }
  RecordCreateHistogramResult(result);
  if (result != CREATE_HISTOGRAM_ALLOCATOR_FULL)
    NOTREACHED() << memory_allocator_->Name() << ", error=" << result;
 
   return nullptr;
 }
