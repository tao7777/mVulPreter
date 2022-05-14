 void CreateDistillerJavaScriptService(
     content::RenderFrameHost* render_frame_host,
    ExternalFeedbackReporter* feedback_reporter,
     mojo::InterfaceRequest<DistillerJavaScriptService> request) {
  new DistillerJavaScriptServiceImpl(render_frame_host, feedback_reporter,
       request.Pass());
 }
