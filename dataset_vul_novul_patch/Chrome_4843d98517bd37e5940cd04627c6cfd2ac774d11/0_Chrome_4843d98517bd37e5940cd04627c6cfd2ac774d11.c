void CorePageLoadMetricsObserver::RecordTimingHistograms(
    const page_load_metrics::PageLoadTiming& timing,
    const page_load_metrics::PageLoadExtraInfo& info) {
  if (info.started_in_foreground && info.first_background_time) {
    const base::TimeDelta first_background_time =
        info.first_background_time.value();

    if (!info.time_to_commit) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramBackgroundBeforeCommit,
                          first_background_time);
    } else if (!timing.first_paint ||
               timing.first_paint > first_background_time) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramBackgroundBeforePaint,
                          first_background_time);
    }
    if (timing.parse_start && first_background_time >= timing.parse_start &&
        (!timing.parse_stop || timing.parse_stop > first_background_time)) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramBackgroundDuringParse,
                          first_background_time);
    }
  }

  if (failed_provisional_load_info_.error != net::OK) {
    DCHECK(failed_provisional_load_info_.interval);

    if (WasStartedInForegroundOptionalEventInForeground(
            failed_provisional_load_info_.interval, info)) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramFailedProvisionalLoad,
                          failed_provisional_load_info_.interval.value());
    }
  }

  if (!info.time_to_commit || timing.IsEmpty())
    return;

  const base::TimeDelta time_to_commit = info.time_to_commit.value();
  if (WasStartedInForegroundOptionalEventInForeground(info.time_to_commit,
                                                      info)) {
    PAGE_LOAD_HISTOGRAM(internal::kHistogramCommit, time_to_commit);
  } else {
    PAGE_LOAD_HISTOGRAM(internal::kBackgroundHistogramCommit, time_to_commit);
  }
  if (timing.dom_content_loaded_event_start) {
    if (WasStartedInForegroundOptionalEventInForeground(
            timing.dom_content_loaded_event_start, info)) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramDomContentLoaded,
                          timing.dom_content_loaded_event_start.value());
      PAGE_LOAD_HISTOGRAM(internal::kHistogramDomLoadingToDomContentLoaded,
                          timing.dom_content_loaded_event_start.value() -
                              timing.dom_loading.value());
    } else {
      PAGE_LOAD_HISTOGRAM(internal::kBackgroundHistogramDomContentLoaded,
                          timing.dom_content_loaded_event_start.value());
    }
  }
  if (timing.load_event_start) {
    if (WasStartedInForegroundOptionalEventInForeground(timing.load_event_start,
                                                        info)) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramLoad,
                          timing.load_event_start.value());
    } else {
      PAGE_LOAD_HISTOGRAM(internal::kBackgroundHistogramLoad,
                          timing.load_event_start.value());
    }
  }
  if (timing.first_layout) {
    if (WasStartedInForegroundOptionalEventInForeground(timing.first_layout,
                                                        info)) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramFirstLayout,
                          timing.first_layout.value());
    } else {
      PAGE_LOAD_HISTOGRAM(internal::kBackgroundHistogramFirstLayout,
                          timing.first_layout.value());
    }
  }
  if (timing.first_paint) {
    if (WasStartedInForegroundOptionalEventInForeground(timing.first_paint,
                                                        info)) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramFirstPaint,
                          timing.first_paint.value());
    } else {
      PAGE_LOAD_HISTOGRAM(internal::kBackgroundHistogramFirstPaint,
                          timing.first_paint.value());
    }

    if (!info.started_in_foreground && info.first_foreground_time &&
        timing.first_paint > info.first_foreground_time.value() &&
        (!info.first_background_time ||
         timing.first_paint < info.first_background_time.value())) {
      PAGE_LOAD_HISTOGRAM(
          internal::kHistogramForegroundToFirstPaint,
          timing.first_paint.value() - info.first_foreground_time.value());
    }
  }
  if (timing.first_text_paint) {
    if (WasStartedInForegroundOptionalEventInForeground(timing.first_text_paint,
                                                        info)) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramFirstTextPaint,
                          timing.first_text_paint.value());
    } else {
      PAGE_LOAD_HISTOGRAM(internal::kBackgroundHistogramFirstTextPaint,
                          timing.first_text_paint.value());
    }
  }
  if (timing.first_image_paint) {
    if (WasStartedInForegroundOptionalEventInForeground(
            timing.first_image_paint, info)) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramFirstImagePaint,
                          timing.first_image_paint.value());
    } else {
      PAGE_LOAD_HISTOGRAM(internal::kBackgroundHistogramFirstImagePaint,
                          timing.first_image_paint.value());
    }
  }
  if (timing.first_contentful_paint) {
    if (WasStartedInForegroundOptionalEventInForeground(
             timing.first_contentful_paint, info)) {
       PAGE_LOAD_HISTOGRAM(internal::kHistogramFirstContentfulPaint,
                           timing.first_contentful_paint.value());
       PAGE_LOAD_HISTOGRAM(
           internal::kHistogramParseStartToFirstContentfulPaint,
           timing.first_contentful_paint.value() - timing.parse_start.value());
      PAGE_LOAD_HISTOGRAM(
          internal::kHistogramDomLoadingToFirstContentfulPaint,
          timing.first_contentful_paint.value() - timing.dom_loading.value());
    } else {
      PAGE_LOAD_HISTOGRAM(internal::kBackgroundHistogramFirstContentfulPaint,
                          timing.first_contentful_paint.value());
    }
  }
  if (timing.parse_start) {
    if (WasParseInForeground(timing.parse_start, timing.parse_stop, info)) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramParseBlockedOnScriptLoad,
                          timing.parse_blocked_on_script_load_duration.value());
      PAGE_LOAD_HISTOGRAM(
          internal::kHistogramParseBlockedOnScriptLoadDocumentWrite,
          timing.parse_blocked_on_script_load_from_document_write_duration
              .value());
    } else {
      PAGE_LOAD_HISTOGRAM(
          internal::kBackgroundHistogramParseBlockedOnScriptLoad,
          timing.parse_blocked_on_script_load_duration.value());
      PAGE_LOAD_HISTOGRAM(
          internal::kBackgroundHistogramParseBlockedOnScriptLoadDocumentWrite,
          timing.parse_blocked_on_script_load_from_document_write_duration
              .value());
    }
  }

  if (timing.parse_stop) {
    base::TimeDelta parse_duration =
        timing.parse_stop.value() - timing.parse_start.value();
    if (WasStartedInForegroundOptionalEventInForeground(timing.parse_stop,
                                                        info)) {
      PAGE_LOAD_HISTOGRAM(internal::kHistogramParseDuration, parse_duration);
      PAGE_LOAD_HISTOGRAM(
          internal::kHistogramParseBlockedOnScriptLoadParseComplete,
          timing.parse_blocked_on_script_load_duration.value());
      PAGE_LOAD_HISTOGRAM(
          internal::
              kHistogramParseBlockedOnScriptLoadDocumentWriteParseComplete,
          timing.parse_blocked_on_script_load_from_document_write_duration
              .value());
    } else {
      PAGE_LOAD_HISTOGRAM(internal::kBackgroundHistogramParseDuration,
                          parse_duration);
      PAGE_LOAD_HISTOGRAM(
          internal::kBackgroundHistogramParseBlockedOnScriptLoadParseComplete,
          timing.parse_blocked_on_script_load_duration.value());
      PAGE_LOAD_HISTOGRAM(
          internal::
              kBackgroundHistogramParseBlockedOnScriptLoadDocumentWriteParseComplete,
          timing.parse_blocked_on_script_load_from_document_write_duration
              .value());
    }
  }

  if (info.started_in_foreground) {
    if (info.first_background_time)
      PAGE_LOAD_HISTOGRAM(internal::kHistogramFirstBackground,
                          info.first_background_time.value());
  } else {
    if (info.first_foreground_time)
      PAGE_LOAD_HISTOGRAM(internal::kHistogramFirstForeground,
                          info.first_foreground_time.value());
  }
}
