void LoadingStatsCollector::RecordPreconnectStats(
    std::unique_ptr<PreconnectStats> stats) {
   const GURL& main_frame_url = stats->url;
   auto it = preconnect_stats_.find(main_frame_url);
   if (it != preconnect_stats_.end()) {
    ReportPreconnectAccuracy(*it->second,
                             std::map<GURL, OriginRequestSummary>());
     preconnect_stats_.erase(it);
   }
 
  preconnect_stats_.emplace(main_frame_url, std::move(stats));
}
