HistogramBase* Histogram::Factory::Build() {
  HistogramBase* histogram = StatisticsRecorder::FindHistogram(name_);
  if (!histogram) {
    const BucketRanges* created_ranges = CreateRanges();
    const BucketRanges* registered_ranges =
        StatisticsRecorder::RegisterOrDeleteDuplicateRanges(created_ranges);

    if (bucket_count_ == 0) {
      bucket_count_ = static_cast<uint32_t>(registered_ranges->bucket_count());
      minimum_ = registered_ranges->range(1);
      maximum_ = registered_ranges->range(bucket_count_ - 1);
    }

    PersistentHistogramAllocator::Reference histogram_ref = 0;
    std::unique_ptr<HistogramBase> tentative_histogram;
    PersistentHistogramAllocator* allocator = GlobalHistogramAllocator::Get();
    if (allocator) {
      tentative_histogram = allocator->AllocateHistogram(
          histogram_type_,
          name_,
          minimum_,
          maximum_,
          registered_ranges,
          flags_,
          &histogram_ref);
    }

    if (!tentative_histogram) {
      DCHECK(!histogram_ref);  // Should never have been set.
      DCHECK(!allocator);  // Shouldn't have failed.
      flags_ &= ~HistogramBase::kIsPersistent;
      tentative_histogram = HeapAlloc(registered_ranges);
      tentative_histogram->SetFlags(flags_);
    }

    FillHistogram(tentative_histogram.get());

    const void* tentative_histogram_ptr = tentative_histogram.get();
    histogram = StatisticsRecorder::RegisterOrDeleteDuplicate(
        tentative_histogram.release());

    if (histogram_ref) {
      allocator->FinalizeHistogram(histogram_ref,
                                   histogram == tentative_histogram_ptr);
    }

    ReportHistogramActivity(*histogram, HISTOGRAM_CREATED);
  } else {
     ReportHistogramActivity(*histogram, HISTOGRAM_LOOKUP);
   }
 
  CHECK_EQ(histogram_type_, histogram->GetHistogramType()) << name_;
   if (bucket_count_ != 0 &&
       !histogram->HasConstructionArguments(minimum_, maximum_, bucket_count_)) {
    DLOG(ERROR) << "Histogram " << name_ << " has bad construction arguments";
    return nullptr;
  }
  return histogram;
}
