void ChromeRenderProcessObserver::OnSetTcmallocHeapProfiling(
    bool profiling, const std::string& filename_prefix) {
#if !defined(OS_WIN)
  if (profiling)
    HeapProfilerStart(filename_prefix.c_str());
  else
    HeapProfilerStop();
#endif
}
