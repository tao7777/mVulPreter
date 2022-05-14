  WARN_UNUSED_RESULT bool VerifyRecordedSamplesForHistogram(
       const size_t num_samples,
       const std::string& histogram_name) const {
     return num_samples ==
            histogram_tester_.GetAllSamples(histogram_name).size();
   }
