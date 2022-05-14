void LoadingStatsCollector::CleanupAbandonedStats() {
   base::TimeTicks time_now = base::TimeTicks::Now();
   for (auto it = preconnect_stats_.begin(); it != preconnect_stats_.end();) {
     if (time_now - it->second->start_time > max_stats_age_) {
      ReportPreconnectAccuracy(*it->second,
                               std::map<GURL, OriginRequestSummary>());
       it = preconnect_stats_.erase(it);
     } else {
       ++it;
    }
  }
}
