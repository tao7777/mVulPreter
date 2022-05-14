bool Job::ResolveDnsNonBlocking(const std::string& host,
                                ResolveDnsOperation op,
                                std::string* output,
                                bool* terminate) {
  CheckIsOnWorkerThread();

  if (abandoned_) {
    return false;
  }

  num_dns_ += 1;

  bool rv;
  if (GetDnsFromLocalCache(host, op, output, &rv)) {
     return rv;
   }
 
  // TODO(eroman): Remove when done gathering data for crbug.com/593759
  if (dont_start_dns_) {
    abandoned_ = true;
    return false;
  }

   if (num_dns_ <= last_num_dns_) {
     ScheduleRestartWithBlockingDns();
    *terminate = true;
    return false;
  }

  if (dns_cache_.size() >= kMaxUniqueResolveDnsPerExec) {
    return false;
  }

  DCHECK(!should_restart_with_blocking_dns_);

  bool completed_synchronously;
  if (!PostDnsOperationAndWait(host, op, &completed_synchronously))
    return false;  // Was cancelled.

  if (completed_synchronously) {
    CHECK(GetDnsFromLocalCache(host, op, output, &rv));
    return rv;
  }

  abandoned_ = true;
  *terminate = true;
  last_num_dns_ = num_dns_;
  return false;
}
