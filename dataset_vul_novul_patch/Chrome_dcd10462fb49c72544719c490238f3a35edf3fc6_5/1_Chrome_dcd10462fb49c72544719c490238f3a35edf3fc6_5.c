 DomDistillerViewerSource::DomDistillerViewerSource(
     DomDistillerServiceInterface* dom_distiller_service,
     const std::string& scheme,
    scoped_ptr<ExternalFeedbackReporter> external_reporter)
     : scheme_(scheme),
       dom_distiller_service_(dom_distiller_service),
      external_feedback_reporter_(external_reporter.Pass()) {
 }
