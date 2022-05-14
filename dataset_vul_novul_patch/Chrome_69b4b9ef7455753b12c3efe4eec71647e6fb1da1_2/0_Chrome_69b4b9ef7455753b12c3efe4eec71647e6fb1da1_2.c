 bool DataReductionProxyConfig::IsFetchInFlight() const {
   DCHECK(thread_checker_.CalledOnValidThread());

  if (!warmup_url_fetcher_)
    return false;
   return warmup_url_fetcher_->IsFetchInFlight();
 }
