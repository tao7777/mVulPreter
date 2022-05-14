void WarmupURLFetcher::FetchWarmupURL(
     size_t previous_attempt_counts,
     const DataReductionProxyServer& proxy_server) {
   DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DCHECK(!params::IsIncludedInHoldbackFieldTrial());
 
   previous_attempt_counts_ = previous_attempt_counts;
 
  DCHECK_LE(0u, previous_attempt_counts_);
  DCHECK_GE(2u, previous_attempt_counts_);

  fetch_delay_timer_.Stop();

  if (previous_attempt_counts_ == 0) {
    FetchWarmupURLNow(proxy_server);
    return;
  }
  fetch_delay_timer_.Start(
      FROM_HERE, GetFetchWaitTime(),
      base::BindOnce(&WarmupURLFetcher::FetchWarmupURLNow,
                     base::Unretained(this), proxy_server));
}
