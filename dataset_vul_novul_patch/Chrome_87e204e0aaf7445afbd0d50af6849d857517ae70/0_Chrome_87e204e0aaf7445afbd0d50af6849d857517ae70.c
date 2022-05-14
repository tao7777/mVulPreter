PerformanceNavigationTiming::PerformanceNavigationTiming(
    LocalFrame* frame,
     ResourceTimingInfo* info,
     TimeTicks time_origin,
     const WebVector<WebServerTimingInfo>& server_timing)
    : PerformanceResourceTiming(
          info ? info->FinalResponse().Url().GetString() : "",
          "navigation",
          time_origin,
          server_timing),
       ContextClient(frame),
       resource_timing_info_(info) {
   DCHECK(frame);
  DCHECK(info);
}
