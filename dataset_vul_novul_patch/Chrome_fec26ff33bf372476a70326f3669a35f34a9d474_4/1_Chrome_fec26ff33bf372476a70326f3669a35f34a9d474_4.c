 void ReportPreconnectAccuracy(
     const PreconnectStats& stats,
    const std::map<GURL, OriginRequestSummary>& requests) {
   if (stats.requests_stats.empty())
     return;
 
  int preresolve_hits_count = 0;
  int preresolve_misses_count = 0;
  int preconnect_hits_count = 0;
  int preconnect_misses_count = 0;

  for (const auto& request_stats : stats.requests_stats) {
    bool hit = requests.find(request_stats.origin) != requests.end();
    bool preconnect = request_stats.was_preconnected;

    preresolve_hits_count += hit;
    preresolve_misses_count += !hit;
    preconnect_hits_count += preconnect && hit;
    preconnect_misses_count += preconnect && !hit;
  }

  int total_preresolves = preresolve_hits_count + preresolve_misses_count;
  int total_preconnects = preconnect_hits_count + preconnect_misses_count;
  DCHECK_EQ(static_cast<int>(stats.requests_stats.size()),
            preresolve_hits_count + preresolve_misses_count);
  DCHECK_GT(total_preresolves, 0);

  size_t preresolve_hits_percentage =
      (100 * preresolve_hits_count) / total_preresolves;

  if (total_preconnects > 0) {
    size_t preconnect_hits_percentage =
        (100 * preconnect_hits_count) / total_preconnects;
    UMA_HISTOGRAM_PERCENTAGE(
        internal::kLoadingPredictorPreconnectHitsPercentage,
        preconnect_hits_percentage);
  }

  UMA_HISTOGRAM_PERCENTAGE(internal::kLoadingPredictorPreresolveHitsPercentage,
                           preresolve_hits_percentage);
  UMA_HISTOGRAM_COUNTS_100(internal::kLoadingPredictorPreresolveCount,
                           total_preresolves);
  UMA_HISTOGRAM_COUNTS_100(internal::kLoadingPredictorPreconnectCount,
                           total_preconnects);
}
