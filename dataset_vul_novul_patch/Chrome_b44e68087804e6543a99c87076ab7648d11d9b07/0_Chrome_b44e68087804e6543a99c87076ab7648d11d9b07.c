void ProfilingService::DumpProcessesForTracing(
     bool keep_small_allocations,
     bool strip_path_from_mapped_files,
     DumpProcessesForTracingCallback callback) {
  if (!helper_) {
    context()->connector()->BindInterface(
        resource_coordinator::mojom::kServiceName, &helper_);
  }

  helper_->GetVmRegionsForHeapProfiler(base::Bind(
      &ProfilingService::OnGetVmRegionsCompleteForDumpProcessesForTracing,
      weak_factory_.GetWeakPtr(), keep_small_allocations,
      strip_path_from_mapped_files, base::Passed(&callback)));
 }
