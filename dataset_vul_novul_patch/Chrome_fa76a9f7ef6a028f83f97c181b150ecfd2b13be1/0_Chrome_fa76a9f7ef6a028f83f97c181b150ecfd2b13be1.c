 CoordinatorImpl::CoordinatorImpl(service_manager::Connector* connector)
     : next_dump_id_(0),
      client_process_timeout_(base::TimeDelta::FromSeconds(15)),
      weak_ptr_factory_(this) {
   process_map_ = std::make_unique<ProcessMap>(connector);
   DCHECK(!g_coordinator_impl);
   g_coordinator_impl = this;
  base::trace_event::MemoryDumpManager::GetInstance()->set_tracing_process_id(
      mojom::kServiceTracingProcessId);

  tracing_observer_ = std::make_unique<TracingObserver>(
      base::trace_event::TraceLog::GetInstance(), nullptr);
}
