 bool DataReductionProxyConfig::IsFetchInFlight() const {
   DCHECK(thread_checker_.CalledOnValidThread());
   return warmup_url_fetcher_->IsFetchInFlight();
 }
