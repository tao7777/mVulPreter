 void OomInterventionImpl::Check(TimerBase*) {
   DCHECK(host_);
 
   OomInterventionMetrics current_memory = GetCurrentMemoryMetrics();
   bool oom_detected = false;

  oom_detected |= detection_args_->blink_workload_threshold > 0 &&
                  current_memory.current_blink_usage_kb * 1024 >
                      detection_args_->blink_workload_threshold;
  oom_detected |= detection_args_->private_footprint_threshold > 0 &&
                  current_memory.current_private_footprint_kb * 1024 >
                      detection_args_->private_footprint_threshold;
  oom_detected |=
      detection_args_->swap_threshold > 0 &&
      current_memory.current_swap_kb * 1024 > detection_args_->swap_threshold;
  oom_detected |= detection_args_->virtual_memory_thresold > 0 &&
                  current_memory.current_vm_size_kb * 1024 >
                      detection_args_->virtual_memory_thresold;

  ReportMemoryStats(current_memory);

  if (oom_detected) {
    if (navigate_ads_enabled_) {
      for (const auto& page : Page::OrdinaryPages()) {
        if (page->MainFrame()->IsLocalFrame()) {
          ToLocalFrame(page->MainFrame())
              ->GetDocument()
              ->NavigateLocalAdsFrames();
        }
      }
    }

    if (renderer_pause_enabled_) {
      pauser_.reset(new ScopedPagePauser);
    }
    host_->OnHighMemoryUsage();
    timer_.Stop();
    V8GCForContextDispose::Instance().SetForcePageNavigationGC();
  }
}
