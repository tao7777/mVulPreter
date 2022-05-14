TracingControllerImpl::TracingControllerImpl()
    : delegate_(GetContentClient()->browser()->GetTracingDelegate()),
      weak_ptr_factory_(this) {
  DCHECK(!g_tracing_controller);
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  base::FileTracing::SetProvider(new FileTracingProviderImpl);
  AddAgents();
   base::trace_event::TraceLog::GetInstance()->AddAsyncEnabledStateObserver(
       weak_ptr_factory_.GetWeakPtr());
   g_tracing_controller = this;

  // TODO(oysteine): Instead of connecting right away, we should connect
  // in StartTracing once this no longer causes test flakiness.
  ConnectToServiceIfNeeded();
 }
