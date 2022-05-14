void MemoryInstrumentation::GetVmRegionsForHeapProfiler(
    RequestGlobalDumpCallback callback) {
  const auto& coordinator = GetCoordinatorBindingForCurrentThread();
  coordinator->GetVmRegionsForHeapProfiler(callback);
}
