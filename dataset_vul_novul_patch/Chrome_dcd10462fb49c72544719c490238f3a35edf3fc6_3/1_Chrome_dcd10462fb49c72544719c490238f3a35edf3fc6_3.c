 DistillerJavaScriptServiceImpl::DistillerJavaScriptServiceImpl(
     content::RenderFrameHost* render_frame_host,
    ExternalFeedbackReporter* external_feedback_reporter,
     mojo::InterfaceRequest<DistillerJavaScriptService> request)
     : binding_(this, request.Pass()),
       render_frame_host_(render_frame_host),
      external_feedback_reporter_(external_feedback_reporter) {}
