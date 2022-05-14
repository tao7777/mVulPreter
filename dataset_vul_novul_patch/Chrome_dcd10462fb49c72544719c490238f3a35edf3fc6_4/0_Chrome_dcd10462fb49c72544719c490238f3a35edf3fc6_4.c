void DistillerJavaScriptServiceImpl::HandleDistillerFeedbackCall(
    bool good) {
  FeedbackReporter::ReportQuality(good);
  if (good) {
    return;
   }
 
  if (!distiller_ui_handle_) {
     return;
   }
   content::WebContents* contents =
       content::WebContents::FromRenderFrameHost(render_frame_host_);
  distiller_ui_handle_->ReportExternalFeedback(
       contents, contents->GetURL(), false);
   return;
 }
