void AutoFillMetrics::Log(QualityMetric metric) const {
   DCHECK(metric < NUM_QUALITY_METRICS);
 
  UMA_HISTOGRAM_ENUMERATION("AutoFill.Quality", metric,
                            NUM_QUALITY_METRICS);
 }
