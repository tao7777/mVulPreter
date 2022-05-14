void Job::ExecuteNonBlocking() {
  CheckIsOnWorkerThread();
  DCHECK(!blocking_dns_);

  if (cancelled_.IsSet())
    return;

  abandoned_ = false;
  num_dns_ = 0;
  alerts_and_errors_.clear();
  alerts_and_errors_byte_cost_ = 0;
  should_restart_with_blocking_dns_ = false;
 
   int result = ExecuteProxyResolver();
 
  // TODO(eroman): Remove when done gathering data for crbug.com/593759
  LogMetricsForBug593759(result);

   if (should_restart_with_blocking_dns_) {
     DCHECK(!blocking_dns_);
     DCHECK(abandoned_);
    blocking_dns_ = true;
    ExecuteBlocking();
    return;
  }

  if (abandoned_)
    return;

  NotifyCaller(result);
}
