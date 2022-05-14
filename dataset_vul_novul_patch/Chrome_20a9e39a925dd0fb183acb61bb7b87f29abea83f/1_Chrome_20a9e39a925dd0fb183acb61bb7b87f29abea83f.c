bool TracingControllerImpl::StartTracing(
    const base::trace_event::TraceConfig& trace_config,
    StartTracingDoneCallback callback) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  if (IsTracing()) {
    if (trace_config.process_filter_config().empty() ||
        trace_config_->process_filter_config().empty()) {
      return false;
    }
    base::trace_event::TraceConfig old_config_copy(*trace_config_);
    base::trace_event::TraceConfig new_config_copy(trace_config);
    old_config_copy.SetProcessFilterConfig(
        base::trace_event::TraceConfig::ProcessFilterConfig());
    new_config_copy.SetProcessFilterConfig(
        base::trace_event::TraceConfig::ProcessFilterConfig());
    if (old_config_copy.ToString() != new_config_copy.ToString())
      return false;
  }
  trace_config_ =
       std::make_unique<base::trace_event::TraceConfig>(trace_config);
 
   start_tracing_done_ = std::move(callback);
  ConnectToServiceIfNeeded();
   coordinator_->StartTracing(trace_config.ToString());
 
   if (start_tracing_done_ &&
      (base::trace_event::TraceLog::GetInstance()->IsEnabled() ||
       !trace_config.process_filter_config().IsEnabled(
           base::Process::Current().Pid()))) {
    std::move(start_tracing_done_).Run();
  }

  return true;
}
