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
 }
