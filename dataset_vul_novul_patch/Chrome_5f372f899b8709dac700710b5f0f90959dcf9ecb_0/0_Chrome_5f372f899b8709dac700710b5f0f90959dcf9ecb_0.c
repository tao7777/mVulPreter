void AutoFillMetrics::Log(QualityMetric metric) const {
void AutoFillMetrics::Log(QualityMetric metric,
                          const std::string& experiment_id) const {
   DCHECK(metric < NUM_QUALITY_METRICS);
 
  std::string histogram_name = "AutoFill.Quality";
  if (!experiment_id.empty())
    histogram_name += "_" + experiment_id;

  UMA_HISTOGRAM_ENUMERATION(histogram_name, metric, NUM_QUALITY_METRICS);
 }
