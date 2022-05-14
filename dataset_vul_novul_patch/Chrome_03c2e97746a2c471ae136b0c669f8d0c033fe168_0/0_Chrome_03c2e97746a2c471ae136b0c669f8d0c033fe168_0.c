HistogramBase* SparseHistogram::FactoryGet(const std::string& name,
                                           int32_t flags) {
  HistogramBase* histogram = StatisticsRecorder::FindHistogram(name);
  if (!histogram) {
    PersistentMemoryAllocator::Reference histogram_ref = 0;
    std::unique_ptr<HistogramBase> tentative_histogram;
    PersistentHistogramAllocator* allocator = GlobalHistogramAllocator::Get();
    if (allocator) {
      tentative_histogram = allocator->AllocateHistogram(
          SPARSE_HISTOGRAM, name, 0, 0, nullptr, flags, &histogram_ref);
    }

    if (!tentative_histogram) {
      DCHECK(!histogram_ref);  // Should never have been set.
      DCHECK(!allocator);      // Shouldn't have failed.
      flags &= ~HistogramBase::kIsPersistent;
      tentative_histogram.reset(new SparseHistogram(name));
      tentative_histogram->SetFlags(flags);
    }

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
 
  CHECK_EQ(SPARSE_HISTOGRAM, histogram->GetHistogramType());
   return histogram;
 }
