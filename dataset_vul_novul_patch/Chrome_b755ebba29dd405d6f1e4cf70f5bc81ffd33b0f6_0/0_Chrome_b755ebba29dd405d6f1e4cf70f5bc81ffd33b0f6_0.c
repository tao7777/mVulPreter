PrintWebViewHelper::PrintWebViewHelper(content::RenderView* render_view)
    : content::RenderViewObserver(render_view),
      content::RenderViewObserverTracker<PrintWebViewHelper>(render_view),
      print_web_view_(NULL),
      is_preview_enabled_(IsPrintPreviewEnabled()),
      is_scripted_print_throttling_disabled_(IsPrintThrottlingDisabled()),
      is_print_ready_metafile_sent_(false),
      ignore_css_margins_(false),
       user_cancelled_scripted_print_count_(0),
       is_scripted_printing_blocked_(false),
       notify_browser_of_print_failure_(true),
      print_for_preview_(false),
      print_node_in_progress_(false) {
 }
