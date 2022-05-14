bool DataReductionProxyConfig::ShouldAddDefaultProxyBypassRules() const {
  DCHECK(thread_checker_.CalledOnValidThread());
  return true;
}
