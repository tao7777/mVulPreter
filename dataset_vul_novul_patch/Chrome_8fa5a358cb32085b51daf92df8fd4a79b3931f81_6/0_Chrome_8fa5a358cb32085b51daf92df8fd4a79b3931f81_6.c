PrintWebViewHelper::PrintWebViewHelper(content::RenderView* render_view,
                                       scoped_ptr<Delegate> delegate)
    : content::RenderViewObserver(render_view),
      content::RenderViewObserverTracker<PrintWebViewHelper>(render_view),
      reset_prep_frame_view_(false),
      is_print_ready_metafile_sent_(false),
      ignore_css_margins_(false),
      is_scripted_printing_blocked_(false),
      notify_browser_of_print_failure_(true),
      print_for_preview_(false),
      delegate_(delegate.Pass()),
       print_node_in_progress_(false),
       is_loading_(false),
       is_scripted_preview_delayed_(false),
      ipc_nesting_level_(0),
       weak_ptr_factory_(this) {
   if (!delegate_->IsPrintPreviewEnabled())
     DisablePreview();
}
