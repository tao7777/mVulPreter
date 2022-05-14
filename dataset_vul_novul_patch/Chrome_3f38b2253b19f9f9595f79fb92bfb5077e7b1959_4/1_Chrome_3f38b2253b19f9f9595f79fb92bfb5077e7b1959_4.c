void PersistentHistogramAllocator::RecordCreateHistogramResult(
    CreateHistogramResultType result) {
  HistogramBase* result_histogram = GetCreateHistogramResultHistogram();
  if (result_histogram)
    result_histogram->Add(result);
}
