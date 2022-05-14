PersistentHistogramAllocator::GetCreateHistogramResultHistogram() {
  constexpr subtle::AtomicWord kHistogramUnderConstruction = 1;
  static subtle::AtomicWord atomic_histogram_pointer = 0;
  subtle::AtomicWord histogram_value =
      subtle::Acquire_Load(&atomic_histogram_pointer);
  if (histogram_value == kHistogramUnderConstruction)
    return nullptr;
  if (histogram_value)
    return reinterpret_cast<HistogramBase*>(histogram_value);
  if (subtle::NoBarrier_CompareAndSwap(&atomic_histogram_pointer, 0,
                                       kHistogramUnderConstruction) != 0) {
    return nullptr;
  }
  if (GlobalHistogramAllocator::Get()) {
    DVLOG(1) << "Creating the results-histogram inside persistent"
             << " memory can cause future allocations to crash if"
             << " that memory is ever released (for testing).";
  }
  HistogramBase* histogram_pointer = LinearHistogram::FactoryGet(
      kResultHistogram, 1, CREATE_HISTOGRAM_MAX, CREATE_HISTOGRAM_MAX + 1,
      HistogramBase::kUmaTargetedHistogramFlag);
  subtle::Release_Store(
      &atomic_histogram_pointer,
      reinterpret_cast<subtle::AtomicWord>(histogram_pointer));
  return histogram_pointer;
}
