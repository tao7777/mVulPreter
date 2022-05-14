 void RenderView::willClose(WebFrame* frame) {
   if (!frame->parent()) {
     const GURL& url = frame->url();
    if (url.SchemeIs("http") || url.SchemeIs("https"))
      DumpLoadHistograms();
  }

  WebDataSource* ds = frame->dataSource();
   NavigationState* navigation_state = NavigationState::FromDataSource(ds);
   navigation_state->user_script_idle_scheduler()->Cancel();
 
  // TODO(jhawkins): Remove once frameDetached is called by WebKit.
   autofill_helper_.FrameWillClose(frame);
 }
