void DistillerJavaScriptServiceImpl::HandleDistillerFeedbackCall(
    bool good) {
  FeedbackReporter::ReportQuality(good);
  if (good) {
    return;
   }
 
  if (!external_feedback_reporter_) {
     return;
   }
   content::WebContents* contents =
       content::WebContents::FromRenderFrameHost(render_frame_host_);
  external_feedback_reporter_->ReportExternalFeedback(
       contents, contents->GetURL(), false);
   return;
 }
